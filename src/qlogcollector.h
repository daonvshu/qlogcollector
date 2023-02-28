#pragma once

#include <qobject.h>
#include <qhash.h>

#include "consolestyle.h"

namespace logcollector {

    class Sender;
    class Cache;
    class QLogCollector : public QObject {
        Q_DISABLE_COPY(QLogCollector)

    public:
        static QLogCollector &instance();

        QLogCollector& registerLog();

        QLogCollector& publishService(int serviceListeningPort = 60025);

        void collectorMessageHandle(QtMsgType type, const QMessageLogContext& context, const QString& msg);

        static void save(const QString& filePath);

    private:
        explicit QLogCollector();

        friend void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    private:
        QHash<Qt::HANDLE, QString> threadNames;

        Sender* sender;
        Cache* cache;
    };

}