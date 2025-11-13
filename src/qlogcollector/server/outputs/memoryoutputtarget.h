#pragma once

#include <qobject.h>

#include "../outputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT MemoryOutputTarget : public OutputTarget {
public:
    explicit MemoryOutputTarget(int limitSize = 3000, bool styledText = false);

    void writePart(const QList<FormatPart>& messageParts, const Message& message) override;

    const QList<Message>& getCaches() const;

    const QStringList& getMessageCaches() const;

    QString base64Encode(const QString& splitChar);

private:
    QList<Message> caches;
    QStringList messageCaches;
    int limitSize;
    bool styledText;
};

QLOGCOLLECTOR_END_NAMESPACE