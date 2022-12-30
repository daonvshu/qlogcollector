#include "qlogcollector.h"

#include "message.h"
#include "sender.h"
#include "console.h"

#include <qmutex.h>
#include <qfileinfo.h>
#include <qthread.h>
#include <qdatetime.h>
#include <qdir.h>

#include "consolestyle.h"

namespace logcollector {

    void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        static QMutex mutex;
        mutex.lock();
        QLogCollector::instance().collectorMessageHandle(type, context, msg);
        mutex.unlock();
    }

    QLogCollector &QLogCollector::init(int serviceListeningPort) {
        auto& i = instance();
        if (!i.sender) {
            i.sender = new Sender(serviceListeningPort, &i);
        }
        return i;
    }

    void QLogCollector::registerLog() {
        qInstallMessageHandler(customMessageHandler);
    }

    void QLogCollector::collectorMessageHandle(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        Message message;
        message.timePoint = QDateTime::currentMSecsSinceEpoch();
        message.category = context.category;

        if (styleConfig.mSimpleCodeLine && styleConfig.mOutputTarget != ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
            message.fileName = QFileInfo(context.file).fileName();
        } else {
            static QDir dir(ROOT_PROJECT_PATH);
            message.fileName = "./" + dir.relativeFilePath(context.file);
        }
        message.codeLine = context.line;

        auto currentThreadId = QThread::currentThreadId();
        message.threadName = threadNames.value(currentThreadId);
        message.threadId = (int64_t)currentThreadId;

        message.level = type;
        message.log = msg;

        sender->appendNewMessage(message);
    }

    QLogCollector::QLogCollector() : sender(nullptr) {
        threadNames.insert(QThread::currentThreadId(), "main");
    }

    QLogCollector &QLogCollector::instance() {
        static QLogCollector logCollector;
        return logCollector;
    }
}
