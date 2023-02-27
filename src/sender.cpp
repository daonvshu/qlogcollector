#include "sender.h"

#include "cache.h"
#include "console.h"
#include "notifier.h"

#include <qcoreapplication.h>

namespace logcollector {

    Sender::Sender(QObject *parent)
        : QObject(parent)
        , notifier(nullptr)
    {
        thread = new QThread(this);
        sendTask = new SendTask;
        sendTask->moveToThread(thread);

        connect(thread, &QThread::finished, sendTask, &SendTask::deleteLater);

        connect(qApp, &QCoreApplication::aboutToQuit, this, [&]{
            if (notifier) {
                notifier->quit();
            }
            thread->quit();
            thread->wait();
        });
    }

    void Sender::publishService(int serviceListeningPort) {
        thread->start();
        sendTask->startTask(serviceListeningPort);

        notifier = new Notifier(serviceListeningPort, this);
        connect(sendTask, &SendTask::deviceInfoBroadcast, notifier, &Notifier::startOrStopNotify);
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
            errStr.prepend("server accept error occur:");
            Console::print(ColorFormatter().setForeground(ColorAttr::Red), errStr);
            Console::endStyle();
        });
        tcpServer->listen(QHostAddress::AnyIPv4, port);
    }

    void SendTask::handleNewConnection() {
        auto client = tcpServer->nextPendingConnection();
        connect(client, &QTcpSocket::readyRead, this, &SendTask::solveClientData);
        connect(client, &QTcpSocket::disconnected, this, &SendTask::handleClientDisconnected);
        clients.append(client);
        deviceInfoBroadcast(false);
        requestSendAllLogs(client);
    }

    void SendTask::solveClientData() {
        auto socket = qobject_cast<QTcpSocket*>(sender());
        auto data = socket->readAll();

    }

    void SendTask::handleClientDisconnected() {
        auto socket = qobject_cast<QTcpSocket*>(sender());
        clients.removeOne(socket);
        if (clients.isEmpty()) {
            deviceInfoBroadcast(true);
        }
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
