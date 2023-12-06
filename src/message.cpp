#include "../include/message.h"

#include <qjsonobject.h>
#include <qjsondocument.h>

QString logcollector::Message::dumpToJson() const {
    QJsonObject obj;
    obj.insert("a", timePoint);
    obj.insert("b", category);
    obj.insert("c", fileName);
    obj.insert("d", codeLine);
    obj.insert("e", threadName);
    obj.insert("f", threadId);
    obj.insert("g", level);
    obj.insert("h", log);
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}
