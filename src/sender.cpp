#include "sender.h"

#include "cache.h"

#include <qcoreapplication.h>

namespace logcollector {

    Sender::Sender(int serviceListeningPort, QObject *parent)
        : QObject(parent)
    {
        thread = new QThread(this);
        sendTask = new SendTask;
        sendTask->moveToThread(thread);

        connect(thread, &QThread::finished, sendTask, &SendTask::deleteLater);
        thread->start();

        sendTask->startTask(serviceListeningPort);

        cache = new Cache(1000, this);
        connect(cache, &Cache::postNewLog, sendTask, &SendTask::sendCache);

        connect(sendTask, &SendTask::requestSendAllLogs, cache, &Cache::packageAllToTarget);

        connect(qApp, &QCoreApplication::aboutToQuit, this, [&]{
            thread->quit();
            thread->wait();
        });
    }

    void Sender::appendNewMessage(Message &message) {
        cache->append(message);
    }

    SendTask::SendTask() {
        connect(this, &SendTask::startTask, this, &SendTask::createListener);
        connect(this, &SendTask::sendCache, this, &SendTask::handleSendCache);
    }

    void SendTask::createListener(int port/*= 60025*/) {
        tcpServer = new QTcpServer(this);
        connect(tcpServer, &QTcpServer::newConnection, this, &SendTask::handleNewConnection);
        connect(tcpServer, &QTcpServer::acceptError, this, [&] (QAbstractSocket::SocketError e) {
            QString errStr;
            QDebug(&errStr) << e;
            auto bytes = errStr.toUtf8();
            printf("server accept error occur: %s", bytes.data());
        });
        tcpServer->listen(QHostAddress::AnyIPv4, port);
    }

    void SendTask::handleNewConnection() {
        auto client = tcpServer->nextPendingConnection();
        connect(client, &QTcpSocket::readyRead, this, &SendTask::solveClientData);
        connect(client, &QTcpSocket::disconnected, this, &SendTask::handleClientDisconnected);
        clients.append(client);
        requestSendAllLogs(client);
    }

    void SendTask::solveClientData() {
        auto socket = qobject_cast<QTcpSocket*>(sender());
        auto data = socket->readAll();

    }

    void SendTask::handleClientDisconnected() {
        auto socket = qobject_cast<QTcpSocket*>(sender());
        clients.removeOne(socket);
        socket->deleteLater();
    }

    void SendTask::handleSendCache(const QByteArray &log, void* socketTarget) {
        if (socketTarget == nullptr) {
            for (const auto &socket: clients) {
                socket->write(log);
            }
        } else {
            auto socket = reinterpret_cast<QTcpSocket*>(socketTarget);
            if (clients.contains(socket)) {
                socket->write(log);
            }
        }
    }
}
