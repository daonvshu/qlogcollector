#include "logcollector.h"
#include "outputstyleconfig.h"
#include "messagehandler.h"

#include <qlogcollector/comm/message.h>

#include <qmutex.h>
#include <qfileinfo.h>
#include <qthread.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qsharedpointer.h>
#include <qcoreapplication.h>

#ifdef Q_OS_LINUX
#include <signal.h>
#include <unistd.h>
#endif

QLOGCOLLECTOR_BEGIN_NAMESPACE

struct LogCollectorData {
    QHash<Qt::HANDLE, QString> threadNames;
    MessageHandler* handler;

    LogCollectorData() {
        threadNames.insert(QThread::currentThreadId(), "main");

        handler = new MessageHandler;
        handler->start();
    }

    void handlerQuit() {
        handler->exit();
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

void LogCollector::addOutputTarget(OutputTarget* outputTarget) {
    if (globalData == nullptr || globalData->handler == nullptr) {
        qFatal("QLogCollector initialization failed, need to call `registerLog` first.");
    }
    globalData->handler->addOutputTarget(outputTarget);
}

void LogCollector::setMessageFormat(const QString& format) {
    if (globalData == nullptr || globalData->handler == nullptr) {
        qFatal("QLogCollector initialization failed, need to call `registerLog` first.");
    }
    globalData->handler->setMessageFormat(format);
}

static bool quitting = false;
void LogCollector::registerLog() {
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
    qInstallMessageHandler(customMessageHandler);
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
    message.log = msg;

    handler->processMessage(message);

    if (type == QtFatalMsg) {
        handler->flush();
        handler->wait(2000);
    }
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
