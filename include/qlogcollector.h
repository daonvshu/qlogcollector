#pragma once

#include <qobject.h>
#include <qhash.h>
#include <qiodevice.h>

#include "consolestyle.h"

namespace logcollector {

    class Cache;
    class QLogCollector : public QObject {
        Q_DISABLE_COPY(QLogCollector)

    public:
        static QLogCollector &instance();

        QLogCollector& registerLog();

        void collectorMessageHandle(QtMsgType type, const QMessageLogContext& context, const QString& msg);

        static void save(QIODevice* device, const QByteArray& splitStr = "\n");

    private:
        explicit QLogCollector();

        friend void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    private:
        QHash<Qt::HANDLE, QString> threadNames;

        Cache* cache;
    };

}