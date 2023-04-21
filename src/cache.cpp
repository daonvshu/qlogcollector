#include "cache.h"
#include "console.h"

#include <qdatetime.h>
#include <qfile.h>

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
        postToLocal(message);
        auto jsonStr = packageMessage(message);
        mutex.lock();
        messages.append(jsonStr);
        if (messages.size() > limitSize) {
            messages.removeFirst();
        }
        mutex.unlock();
        postNewLog(jsonStr, nullptr);
    }

    void Cache::packageAllToTarget(void* socketTarget) {
        QByteArray historyLogs;
        mutex.lock();
        for (auto& message: messages) {
            historyLogs.append(message);
        }
        mutex.unlock();
        postNewLog(historyLogs, socketTarget);
    }

    void Cache::save2File(const QString &filePath) {
        QFile logFile(filePath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            for (auto& message: messages) {
                logFile.write(message);
            }
            logFile.flush();
            logFile.close();
        }
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
        char* compressBuf = (char*) malloc(bytes.size() * 2);
        auto compressedLen = unishox2_compress_simple(bytes.data(), bytes.size(), compressBuf);
        compressBuf[compressedLen] = '\0';
        QByteArray compressedBytes(compressBuf, compressedLen + 1);
        free(compressBuf);
        return compressedBytes.toBase64() + ",";
    }
}
