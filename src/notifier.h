#pragma once

#include <qobject.h>
#include <qudpsocket.h>
#include <qtimer.h>

namespace logcollector {

    class Notifier : public QObject {
    public:
        static Notifier* init(int sendPort, QObject* parent);

        static void startOrStopNotify(bool start = true);

    private:
        explicit Notifier(int sendPort, QObject* parent);

        void notifyBaseInfo();

    private:
        static Notifier* instance;

        QUdpSocket* udpSocket;
        QTimer* notifyTimer;

        int mSendPort;
    };

}
