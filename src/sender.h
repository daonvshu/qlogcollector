#pragma once

#include <qobject.h>
#include <qthread.h>
#include <qtcpserver.h>
#include <qtcpsocket.h>

#include "message.h"

namespace logcollector {

    class SendTask;
    class Cache;
    class Sender : public QObject {
    public:
        explicit Sender(QObject* parent = nullptr);

        void appendNewMessage(Message& message);

    private:
        QThread* thread;
        SendTask* sendTask;

        Cache* cache;
    };

    class SendTask : public QObject {
        Q_OBJECT

    public:
        explicit SendTask();

    signals:
        void startTask();
        void sendCache(const QByteArray& log, void* socketTarget);
        void requestSendAllLogs(void* socketTarget);

    private slots:
        void createListener();
        void handleNewConnection();
        void solveClientData();
        void handleClientDisconnected();
        void handleSendCache(const QByteArray& log, void* socketTarget);

    private:
        QTcpServer* tcpServer;
        QList<QTcpSocket*> clients;
    };
}