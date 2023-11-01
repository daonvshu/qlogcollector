#include "notifier.h"
#include "message.h"

extern "C" {
#include <unishox2.h>
}

#include <qsysinfo.h>
#include <qcoreapplication.h>
#include <qnetworkinterface.h>

namespace logcollector {

    Notifier::Notifier(int sendPort, QObject* parent)
        : QObject(parent)
    {
        thread = new QThread(this);
        worker = new NotifierWorker;
        worker->moveToThread(thread);

        connect(thread, &QThread::finished, worker, &NotifierWorker::deleteLater);
        thread->start();

        worker->createService(sendPort);
    }

    void Notifier::startOrStopNotify(bool start) {
        if (start) {
            worker->startNotify();
        } else {
            worker->stopNotify();
        }
    }

    void Notifier::quit() {
        thread->quit();
        thread->wait();
    }

    NotifierWorker::NotifierWorker() {
        connect(this, &NotifierWorker::createService, this, &NotifierWorker::createUdpService, Qt::QueuedConnection);
    }

    void NotifierWorker::createUdpService(int sendPort) {
        mSendPort = sendPort;

        notifyTimer = new QTimer(this);
        notifyTimer->setInterval(1000);
        notifyTimer->setSingleShot(false);
        connect(notifyTimer, &QTimer::timeout, this, &NotifierWorker::notifyBaseInfo, Qt::QueuedConnection);
        notifyTimer->start();

        udpSocket = new QUdpSocket(this);

        connect(this, &NotifierWorker::startNotify, notifyTimer, QOverload<>::of(&QTimer::start), Qt::QueuedConnection);
        connect(this, &NotifierWorker::stopNotify, notifyTimer, &QTimer::stop, Qt::QueuedConnection);
    }

    void NotifierWorker::notifyBaseInfo() {
        BroadcastInfo broadcastInfo;

        QString sysInfo = "%1/%2 %3 %4 (%5)";
        sysInfo = sysInfo.arg(QSysInfo::prettyProductName(), QSysInfo::kernelType(), QSysInfo::kernelVersion(),
                              QSysInfo::currentCpuArchitecture(), QSysInfo::machineHostName());
        broadcastInfo.sysInfo = sysInfo;

        broadcastInfo.processId = QCoreApplication::applicationPid();

        auto jsonStr = QJsonDocument(broadcastInfo.dumpToJson()).toJson(QJsonDocument::Compact);

        auto bytes = jsonStr;
        bytes.append('\0');
        char* compressBuf = (char*) malloc(bytes.size());
        auto compressedLen = unishox2_compress_simple(bytes.data(), bytes.size(), compressBuf);
        QByteArray compressedBytes(compressBuf, compressedLen);
        free(compressBuf);
        compressedBytes = compressedBytes.toBase64();

        for(const auto& interfaces: QNetworkInterface::allInterfaces()) {
            for(const auto& entry: interfaces.addressEntries()) {
                QHostAddress broadcastAddress = entry.broadcast();
                if (broadcastAddress != QHostAddress::Null
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                    && !entry.ip().isLinkLocal()
#endif
                    && entry.ip() != QHostAddress::LocalHost
                    && entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                {
                    udpSocket->writeDatagram(compressedBytes.data(), compressedBytes.size(), broadcastAddress, mSendPort);
                }
            }
        }
    }
}