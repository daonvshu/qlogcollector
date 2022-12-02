#include "cache.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <qdatetime.h>
#include <qdebug.h>

extern "C" {
#include <unishox2.h>
}

#ifdef Q_CC_MSVC
#include <Windows.h>
#endif

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

        auto dateTime = QDateTime::fromMSecsSinceEpoch(message.timePoint());
        std::cout << dateTime.toString("HH:mm:ss.zzz").toStdString() << "   ";

        if (!message.threadName().isEmpty()) {
            std::cout << message.threadName().leftJustified(14, ' ', true).toStdString();
        } else {
            std::cout << QString::number(message.threadId()).leftJustified(14, ' ', true).toStdString();
        }
        std::cout << "   ";

        if (message.category() == "default") {
            std::cout << QString("<no-category>").leftJustified(24, ' ', true).toStdString();
        } else {
            std::cout << message.category().leftJustified(24, ' ', true).toStdString();
        }
        std::cout << "   ";

#if defined Q_CC_MSVC
        WORD wOldColorAttrs;
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(handle, &bufferInfo);
        wOldColorAttrs = bufferInfo.wAttributes;
#endif
        
        auto msgType = (QtMsgType)message.level();
        switch (msgType) {
#if defined Q_CC_MSVC
            case QtDebugMsg:
                SetConsoleTextAttribute(handle, 0x2f);
                break;
            case QtInfoMsg:
                SetConsoleTextAttribute(handle, 0x1f);
                break;
            case QtWarningMsg:
                SetConsoleTextAttribute(handle, 0x60);
                break;
            case QtCriticalMsg:
            case QtFatalMsg:
                SetConsoleTextAttribute(handle, 0x40);
                break;
#elif defined Q_CC_GNU
            case QtDebugMsg:
                std::cout << "\033[42m";
                break;
            case QtInfoMsg:
                std::cout << "\033[44m";
                break;
            case QtWarningMsg:
                std::cout << "\033[43;30m";
                break;
            case QtCriticalMsg:
            case QtFatalMsg:
                std::cout << "\033[41;30m";
                break;
#endif
        }

        switch (msgType) {
            case QtDebugMsg:
                std::cout << " D ";
                break;
            case QtWarningMsg:
                std::cout << " W ";
                break;
            case QtInfoMsg:
                std::cout << " I ";
                break;
            case QtCriticalMsg:
            case QtFatalMsg:
                std::cout << " E ";
                break;
        }

#if defined Q_CC_MSVC
        SetConsoleTextAttribute(handle, wOldColorAttrs);
#elif defined Q_CC_GNU
        std::cout << "\033[0m";
#endif

        std::cout << " " << message.log().toUtf8().toStdString() << " ";

#if defined Q_CC_MSVC
        SetConsoleTextAttribute(handle, 0x8001);
#elif defined Q_CC_GNU
        std::cout << "\033[4;34m";
#endif
        QString line = "(" + message.fileName() + ":" + QString::number(message.codeLine()) + ")";
        std::cout << line.toStdString();

#if defined Q_CC_MSVC
        SetConsoleTextAttribute(handle, wOldColorAttrs);
#elif defined Q_CC_GNU
        std::cout << "\033[0m";
#endif

        std::cout << std::endl;
    }

    QByteArray Cache::packageMessage(Message& message) {
        auto json = message.dumpToJson();
        QJsonDocument doc(json);
        auto jsonStr = doc.toJson(QJsonDocument::Compact);
        //base64
        auto bytes = jsonStr;
        char* compressBuf = (char*) malloc(bytes.size());
        auto compressedLen = unishox2_compress_simple(bytes.data(), bytes.size(), compressBuf);
        QByteArray compressedBytes(compressBuf, compressedLen);
        free(compressBuf);
        return compressedBytes.toBase64() + ",";
    }


}