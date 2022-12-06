#include "qlogcollector.h"

#include "message.h"
#include "sender.h"
#include "console.h"

#include <qmutex.h>
#include <qfileinfo.h>
#include <qthread.h>
#include <qdatetime.h>

namespace logcollector {

    void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        static QMutex mutex;
        mutex.lock();
        QLogCollector::instance().collectorMessageHandle(type, context, msg);
        mutex.unlock();
    }

    QLogCollector &QLogCollector::init() {
        return instance();
    }

    void QLogCollector::registerLinuxOutput() {
        Console::resetConsoleOutputTarget(ConsoleOutputTarget::TARGET_STANDARD_OUTPUT);
    }

    void QLogCollector::registerWin32ConsoleAppOutput() {
        Console::resetConsoleOutputTarget(ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP);
    }

    void QLogCollector::registerWin32DebugConsoleOutput() {
        Console::resetConsoleOutputTarget(ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE);
    }

    void QLogCollector::registerLog() {
        qInstallMessageHandler(customMessageHandler);
    }

    void QLogCollector::collectorMessageHandle(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        Message message;
        message.timePoint = QDateTime::currentMSecsSinceEpoch();
        message.category = context.category;

        QFileInfo info(context.file);
        message.fileName = info.fileName();
        message.codeLine = context.line;

        auto currentThreadId = QThread::currentThreadId();
        message.threadName = threadNames.value(currentThreadId);
        message.threadId = (int64_t)currentThreadId;

        message.level = type;
        message.log = msg;

        sender->appendNewMessage(message);
    }

    QLogCollector::QLogCollector() {
        sender = new Sender(this);
        threadNames.insert(QThread::currentThreadId(), "main");
    }

    QLogCollector &QLogCollector::instance() {
        static QLogCollector logCollector;
        return logCollector;
    }
}