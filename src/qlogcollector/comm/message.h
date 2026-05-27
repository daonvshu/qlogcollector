#pragma once

#include <qobject.h>
#include <qjsonobject.h>
#include <qjsondocument.h>

#include "global.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

struct Message {
    qint64 timePoint;
    QString category;
    QString fileName;
    int codeLine;
    QString threadName;
    qint64 threadId;
    int level;
    QString log;
    QString traceId;
    QString traceContextBase64;

    QString dumpToJson() const {
        QJsonObject obj;
        obj.insert("a", timePoint);
        obj.insert("b", category);
        obj.insert("c", fileName);
        obj.insert("d", codeLine);
        obj.insert("e", threadName);
        obj.insert("f", threadId);
        obj.insert("g", level);
        obj.insert("h", log);
        obj.insert("i", traceId);
        obj.insert("j", traceContextBase64);
        return QJsonDocument(obj).toJson(QJsonDocument::Compact);
    }
};

QLOGCOLLECTOR_END_NAMESPACE
