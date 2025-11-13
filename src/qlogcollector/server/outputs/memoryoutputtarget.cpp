#include "memoryoutputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

MemoryOutputTarget::MemoryOutputTarget(int limitSize, bool styledText)
    : limitSize(limitSize)
    , styledText(styledText)
{}

void MemoryOutputTarget::writePart(const QList<FormatPart>& messageParts, const Message& message) {
    caches.append(message);
    while (caches.size() > limitSize) {
        caches.removeFirst();
    }

    QString messageCache;
    for (const auto& part : messageParts) {
        if (styledText) {
            auto content = part.content;

            if (part.type == FormatPart::Type::Level) {
                content = TextUtils::getLevelCodeFormatter(content).toStdColorCode() + content + "\033[0m";
            } else if (part.type == FormatPart::Type::File) {
                content = TextUtils::getFileCodeFormatter(part.colorFormatter).toStdColorCode() + content + "\033[0m";
            } else {
                if (part.colorFormatter != nullptr && !part.colorFormatter->isInvalid()) {
                    content = part.colorFormatter->toStdColorCode() + content + "\033[0m";
                }
            }
            messageCache += content;
        } else {
            auto content = part.content;
            if (part.type == FormatPart::Type::Message) {
                content = TextUtils::removeColors(content);
            }
            messageCache += content;
        }
        if (part.lineBreak) {
            messageCache += "\n";
        }
    }
    messageCaches << messageCache;
    while (messageCaches.size() > limitSize) {
        messageCaches.removeFirst();
    }
}

const QList<Message>& MemoryOutputTarget::getCaches() const {
    return caches;
}

const QStringList& MemoryOutputTarget::getMessageCaches() const {
    return messageCaches;
}

QString MemoryOutputTarget::base64Encode(const QString& splitChar) {
    QStringList messages;
    for (const auto& message : caches) {
        messages.append(message.dumpToJson().toUtf8().toBase64());
    }
    return messages.join(splitChar);
}

QLOGCOLLECTOR_END_NAMESPACE
