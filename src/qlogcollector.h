#pragma once

#include <qobject.h>
#include <qhash.h>

#include "consolestyle.h"

namespace logcollector {

    class Sender;
    class QLogCollector : public QObject {
        Q_DISABLE_COPY(QLogCollector)

    public:
        static QLogCollector &init();

        void registerLog();

        void collectorMessageHandle(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    private:
        explicit QLogCollector();

        static QLogCollector &instance();

        friend void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    private:
        QHash<Qt::HANDLE, QString> threadNames;

        Sender* sender;
    };

}