#pragma once

#include <qobject.h>
#include <qmutex.h>

#include "message.h"

namespace logcollector {

    class Cache : public QObject {
        Q_OBJECT

    public:
        explicit Cache(int cacheMaxSize, QObject* parent = nullptr);

        void append(Message& message);

        void packageAllToTarget(void* socketTarget);

    signals:
        void postNewLog(const QByteArray& log, void* socketTarget);

    private:
        QMutex mutex;
        int limitSize;
        QList<Message> messages;

    private:
        static void postToLocal(Message& message);
        static QByteArray packageMessage(Message& message);
    };

}
