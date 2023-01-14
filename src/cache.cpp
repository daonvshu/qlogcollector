#include "cache.h"
#include "console.h"

#include <qdatetime.h>
#include <qdebug.h>

extern "C" {
#include <unishox2.h>
}

namespace logcollector {


    Cache::Cache(int cacheMaxSize, QObject* parent)
        : QObject(parent)
        , limitSize(cacheMaxSize)
    {

    }

    void Cache::append(Message &message) {
        mutex.lock();
        messages.append(message);
        if (messages.size() > limitSize) {
            messages.removeFirst();
        }
        mutex.unlock();

        postToLocal(message);
        auto jsonStr = packageMessage(message);
        postNewLog(jsonStr, nullptr);
    }

    void Cache::packageAllToTarget(void* socketTarget) {
        QByteArray historyLogs;
        mutex.lock();
        for (auto& message: messages) {
            historyLogs.append(packageMessage(message));
        }
        mutex.unlock();
        postNewLog(historyLogs, socketTarget);
    }

    void Cache::postToLocal(Message &message) {
        Console::printMessage(message);
    }

    QByteArray Cache::packageMessage(Message& message) {
        auto json = message.dumpToJson();
        QJsonDocument doc(json);
        auto jsonStr = doc.toJson(QJsonDocument::Compact);
        //base64
        auto bytes = jsonStr;
        char* compressBuf = (char*) malloc(bytes.size());
        auto compressedLen = unishox2_compress_simple(bytes.data(), bytes.size(), compressBuf);
        compressBuf[compressedLen] = '\0';
        QByteArray compressedBytes(compressBuf, compressedLen + 1);
        free(compressBuf);
        return compressedBytes.toBase64() + ",";
    }
}
