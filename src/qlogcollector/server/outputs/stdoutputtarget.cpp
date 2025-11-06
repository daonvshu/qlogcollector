#include "stdoutputtarget.h"

#include <iostream>

QLOGCOLLECTOR_BEGIN_NAMESPACE

void StdOutputTarget::writePart(const QList<FormatPart>& messageParts, const Message&) {
    auto parts = TextUtils::wordwrap(messageParts);
    for (auto part : parts) {
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

        if (isSystemCodePage()) {
            std::cout << content.toLocal8Bit().data();
        } else {
            std::cout << content.toStdString();
        }
        if (part.lineBreak) {
            std::cout << std::endl;
        }
    }
    if (!parts.isEmpty() && !parts.last().lineBreak) {
        std::cout << std::endl;
    }
}

QLOGCOLLECTOR_END_NAMESPACE
