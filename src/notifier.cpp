#include "notifier.h"
#include "message.h"

extern "C" {
#include <unishox2.h>
}

#include <qsysinfo.h>
#include <qcoreapplication.h>
#include <qnetworkinterface.h>

namespace logcollector {

    Notifier* Notifier::instance = nullptr;

    Notifier* Notifier::init(int sendPort, QObject* parent) {
        if (instance == nullptr) {
            instance = new Notifier(sendPort, parent);
        }
        return instance;
    }

    void Notifier::startOrStopNotify(bool start) {
        if (start) {
            instance->notifyTimer->start();
        } else {
            instance->notifyTimer->stop();
        }
    }

    Notifier::Notifier(int sendPort, QObject* parent)
        : QObject(parent)
        , mSendPort(sendPort)
    {
        notifyTimer = new QTimer(this);
        notifyTimer->setInterval(1000);
        notifyTimer->setSingleShot(false);

        connect(notifyTimer, &QTimer::timeout, this, &Notifier::notifyBaseInfo);

        udpSocket = new QUdpSocket(this);
    }

    void Notifier::notifyBaseInfo() {
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
                    && !entry.ip().isLinkLocal()
                    && entry.ip() != QHostAddress::LocalHost
                    && entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                {
                    udpSocket->writeDatagram(compressedBytes.data(), compressedBytes.size(), broadcastAddress, mSendPort);
                }
            }
        }
    }
}