#pragma once

#include <qobject.h>

namespace logcollector {

    struct Message {
        qint64 timePoint;
        QString category;
        QString fileName;
        int codeLine;
        QString threadName;
        qint64 threadId;
        int level;
        QString log;

        QString dumpToJson() const;
    };
}