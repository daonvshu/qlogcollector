#include "../include/qlogcollector.h"

#include "../include/message.h"
#include "../include/cache.h"
#include "../include/console.h"

#include <qmutex.h>
#include <qfileinfo.h>
#include <qthread.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qcoreapplication.h>

namespace logcollector {

    void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        static QMutex mutex;
        mutex.lock();
        QLogCollector::instance().collectorMessageHandle(type, context, msg);
        mutex.unlock();
    }

    QLogCollector& QLogCollector::registerLog() {
        qInstallMessageHandler(customMessageHandler);
        return *this;
    }

    void QLogCollector::collectorMessageHandle(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        Message message;
        message.timePoint = QDateTime::currentMSecsSinceEpoch();
        message.category = context.category;

        if (styleConfig.mSimpleCodeLine && styleConfig.mOutputTarget != ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
            message.fileName = QFileInfo(context.file).fileName();
        } else {
            QDir dir(ROOT_PROJECT_PATH);
            message.fileName = "./" + dir.relativeFilePath(context.file);
        }
        message.codeLine = context.line;

        auto currentThreadId = QThread::currentThreadId();
        message.threadName = threadNames.value(currentThreadId);
        message.threadId = (int64_t)currentThreadId;

        message.level = type;
        message.log = msg;

        cache->append(message);
    }

    QLogCollector::QLogCollector() {
        cache = new Cache(1000, this);
        threadNames.insert(QThread::currentThreadId(), "main");
    }

    QLogCollector &QLogCollector::instance() {
        static QLogCollector logCollector;
        return logCollector;
    }

    void QLogCollector::save(QIODevice* device, const QByteArray& splitStr) {
        instance().cache->save2Device(device, splitStr);
    }
}
