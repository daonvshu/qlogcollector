#include "logcollector.h"
#include "outputstyleconfig.h"
#include "messagehandler.h"
#include "tracescope.h"
#include "outputs/fileoutputtarget.h"
#include "outputs/traceroutputtarget.h"

#include <qlogcollector/comm/message.h>

#include <qmutex.h>
#include <qfileinfo.h>
#include <qthread.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qsharedpointer.h>
#include <qcoreapplication.h>
#include <qatomic.h>

#ifdef Q_OS_LINUX
#include <signal.h>
#include <unistd.h>
#endif

QLOGCOLLECTOR_BEGIN_NAMESPACE

struct LogCollectorData {
    QHash<Qt::HANDLE, QString> threadNames;
    MessageHandler* handler;
    FileOutputTarget* fileOutputTarget = nullptr;
    TracerOutputTarget* tracerOutputTarget = nullptr;
    QAtomicInteger<qint64> traceSeq = 0;

    LogCollectorData() {
        threadNames.insert(QThread::currentThreadId(), "main");

        handler = new MessageHandler;
        handler->start();
    }

    void handlerQuit() {
        handler->exit();
        if (QThread::currentThread() == handler) {
            handler->requestInterruption();
            return;
        }
        if (!handler->wait(3000)) {
            handler->terminate();
            handler->wait();
        }
        delete handler;
        handler = nullptr;
    }
};

static LogCollectorData* globalData = nullptr;

void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    LogCollector::collectorMessageHandle(type, context, msg);
}

OutputStyleConfig LogCollector::styleConfig;

static bool quitting = false;
void LogCollector::init() {
    if (globalData != nullptr) {
        return;
    }
    globalData = new LogCollectorData;
    connect(qApp, &QCoreApplication::aboutToQuit, [] {
        if (quitting) return;
        quitting = true;

        qInstallMessageHandler(nullptr);
        if (globalData && globalData->handler) {
            globalData->handlerQuit();
            delete globalData;
            globalData = nullptr;
        }
    });
}

void LogCollector::registerLog() {
    init();
    qInstallMessageHandler(customMessageHandler);
}

void LogCollector::addOutputTarget(OutputTarget* outputTarget) {
    if (globalData == nullptr || globalData->handler == nullptr) {
        qFatal("QLogCollector initialization failed, need to call `init` or `registerLog` first.");
    }
    if (auto* fileTarget = dynamic_cast<FileOutputTarget*>(outputTarget)) {
        globalData->fileOutputTarget = fileTarget;
        if (globalData->tracerOutputTarget) {
            globalData->tracerOutputTarget->followFileOutputConfig(fileTarget->outputConfig());
        }
    } else if (auto* tracerTarget = dynamic_cast<TracerOutputTarget*>(outputTarget)) {
        globalData->tracerOutputTarget = tracerTarget;
        if (globalData->fileOutputTarget) {
            tracerTarget->followFileOutputConfig(globalData->fileOutputTarget->outputConfig());
        }
    }
    globalData->handler->addOutputTarget(outputTarget);
}

void LogCollector::setMessageFormat(const QString& format) {
    if (globalData == nullptr || globalData->handler == nullptr) {
        qFatal("QLogCollector initialization failed, need to call `init` or `registerLog` first.");
    }
    globalData->handler->setMessageFormat(format);
}

void LogCollector::collectorMessageHandle(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if (!globalData) {
        return;
    }
    auto handler = globalData->handler;
    if (handler == nullptr) {
        return;
    }

    Message message;
    message.timePoint = QDateTime::currentMSecsSinceEpoch();
    message.category = context.category;

    if (styleConfig.mSimpleCodeLine) {
        message.fileName = QFileInfo(context.file).fileName();
    } else {
        if (styleConfig.codeRootPath.isEmpty()) {
            message.fileName = context.file;
        } else {
            QDir dir(styleConfig.codeRootPath);
            message.fileName = "./" + dir.relativeFilePath(context.file);
        }
    }
    message.codeLine = context.line;

    auto currentThreadId = QThread::currentThreadId();
    message.threadName = globalData->threadNames.value(currentThreadId);
    message.threadId = (int64_t)currentThreadId;

    message.level = type;
    if (handler->isTraceCollectionEnabled()) {
        auto baseTraceId = currentTraceId();
        if (!baseTraceId.isEmpty()) {
            const auto seq = globalData->traceSeq.fetchAndAddOrdered(1) + 1;
            message.traceId = baseTraceId + QStringLiteral("_") + QString::number(seq);
            message.traceContextBase64 = currentTraceContextBase64(message.traceId);
        }
    }
    message.log = msg;
    if (!message.traceId.isEmpty()) {
        message.log += QStringLiteral(" [trace_id=") + message.traceId + QStringLiteral("]");
    }

    handler->processMessage(message);

    if (type == QtFatalMsg) {
        handler->flush();
        if (QThread::currentThread() != handler) {
            handler->wait(2000);
        }
    }
}

QString LogCollector::exportTraceContext() {
    return QLogCollector::exportTraceContext();
}

void LogCollector::importTraceContext(const QString& context) {
    QLogCollector::importTraceContext(context);
}

void LogCollector::clearTraceContext() {
    QLogCollector::clearTraceContext();
}

void LogCollector::flushLogs() {
    if (globalData && globalData->handler) {
        globalData->handler->flush();
    }
}

#ifdef Q_OS_LINUX
static void crashSignalHandler(int sig) {
    const char* msg = "Program crashed! Trying to flush logs...\n";
    ::write(STDERR_FILENO, msg, strlen(msg));
    LogCollector::flushLogs();
    signal(SIGSEGV, SIG_DFL);
    raise(SIGSEGV);
}
#endif

void LogCollector::bindSignalFatal() {
#ifdef Q_OS_LINUX
    signal(SIGSEGV, crashSignalHandler);
    signal(SIGABRT, crashSignalHandler);
#endif
}

QLOGCOLLECTOR_END_NAMESPACE
