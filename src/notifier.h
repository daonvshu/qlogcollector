#pragma once

#include <qobject.h>
#include <qudpsocket.h>
#include <qtimer.h>
#include <qthread.h>

namespace logcollector {

    class NotifierWorker : public QObject {
        Q_OBJECT

    public:
        explicit NotifierWorker();

    signals:
        void createService(int sendPort);

        void startNotify();
        void stopNotify();

    private:
        QUdpSocket* udpSocket = nullptr;
        QTimer* notifyTimer = nullptr;

        int mSendPort = 0;

    private:
        void createUdpService(int sendPort);

        void notifyBaseInfo();
    };

    class Notifier : public QObject {
    public:
        explicit Notifier(int sendPort, QObject* parent);

        void startOrStopNotify(bool start = true);

        void quit();

    private:
        QThread* thread;
        NotifierWorker* worker;
    };

}
