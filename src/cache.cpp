#include "../include/cache.h"
#include "../include/console.h"

namespace logcollector {


    Cache::Cache(int cacheMaxSize, QObject* parent)
        : QObject(parent)
        , limitSize(cacheMaxSize)
    {
    }

    void Cache::append(Message &message) {
        postToLocal(message);
        auto jsonStr = message.dumpToJson().toUtf8().toBase64();
        mutex.lock();
        messages.append(jsonStr);
        if (messages.size() > limitSize) {
            messages.removeFirst();
        }
        mutex.unlock();
    }

    void Cache::save2Device(QIODevice *device, const QByteArray& splitStr) {
        mutex.lock();
        for (int i = 0; i < messages.size(); i++) {
            device->write(messages[i]);
            if (i < messages.size() - 1) {
                device->write(splitStr);
            }
        }
        mutex.unlock();
    }

    void Cache::postToLocal(Message &message) {
        Console::printMessage(message);
    }
}
