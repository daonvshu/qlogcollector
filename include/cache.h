#pragma once

#include <qobject.h>
#include <qmutex.h>
#include <qiodevice.h>

#include "message.h"

namespace logcollector {

    class Cache : public QObject {
    public:
        explicit Cache(int cacheMaxSize, QObject* parent = nullptr);

        void append(Message& message);

        void save2Device(QIODevice* device, const QByteArray& splitStr);

    private:
        QMutex mutex;
        int limitSize;
        QList<QByteArray> messages;

    private:
        static void postToLocal(Message& message);
    };

}
