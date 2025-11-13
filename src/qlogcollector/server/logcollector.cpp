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
    QSharedPointer<MessageHandler> handler;

    LogCollectorData() {
        threadNames.insert(QThread::currentThreadId(), "main");

        handler = QSharedPointer<MessageHandler>(new MessageHandler);
        handler->start();
        QObject::connect(qApp, &QCoreApplication::aboutToQuit, [this] {
            handler->exit();
            handler->wait();
        });
    }
};

void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    LogCollector::collectorMessageHandle(type, context, msg);
}

OutputStyleConfig LogCollector::styleConfig;

namespace {
    LogCollectorData& globalData() {
        static LogCollectorData data;
        return data;
    }
}

void LogCollector::addOutputTarget(OutputTarget* outputTarget) {
    globalData().handler->addOutputTarget(outputTarget);
}

void LogCollector::setMessageFormat(const QString& format) {
    globalData().handler->setMessageFormat(format);
}

void LogCollector::registerLog() {
    qInstallMessageHandler(customMessageHandler);
}

void LogCollector::collectorMessageHandle(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
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
    message.threadName = globalData().threadNames.value(currentThreadId);
    message.threadId = (int64_t)currentThreadId;

    message.level = type;
    message.log = msg;

    globalData().handler->processMessage(message);

    if (type == QtFatalMsg) {
        globalData().handler->flush();
        globalData().handler->wait(2000);
    }
}

void LogCollector::flushLogs() {
    globalData().handler->flush();
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
