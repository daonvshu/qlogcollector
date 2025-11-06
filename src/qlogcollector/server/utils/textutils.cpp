#include "textutils.h"

#include "../logcollector.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

QList<FormatPart> TextUtils::wordwrap(const QList<FormatPart>& messageParts) {
    auto widthLimit = LogCollector::styleConfig.mLogLineWidth;
    if (widthLimit <= 0) {
        return messageParts;
    }
    QList<FormatPart> result;
    int prefixHolder = 0;
    int contentIndex = -1;
    for (auto& part : messageParts) {
        contentIndex++;
        if (part.type == FormatPart::Type::Message) {
            break;
        }
        prefixHolder += part.contentLength;
        result << part;
    }
    if (prefixHolder >= widthLimit) {
        return messageParts;
    }

    int contentLimit = widthLimit - prefixHolder;
    auto contentParts = toContentParts(messageParts[contentIndex].content);
    int length = 0;
    for (const auto& part : contentParts) {
        if (part.isStyleCode) {
            result << FormatPart{ FormatPart::Type::Message, "\x1b[" + part.part + "m" };
            continue;
        }
        if (length + part.part.length() <= contentLimit) {
            bool lineBreak = length + part.part.length() == contentLimit;
            result << FormatPart{ FormatPart::Type::Message, part.part, lineBreak };
            if (lineBreak) {
                length = 0;
            } else {
                length += part.part.length();
            }
            continue;
        }

        auto text = part.part;
        int remain = contentLimit - length;

        while (!text.isEmpty()) {
            int take = qMin(remain, text.length());
            auto chunk = text.left(take);
            if (!result.isEmpty() && result.last().lineBreak) {
                chunk.prepend(QString(prefixHolder, ' '));
            }

            bool shouldBreakLine = (take == remain);
            result.append({ FormatPart::Type::Message, chunk, shouldBreakLine });

            text.remove(0, take);

            if (shouldBreakLine) {
                length = 0;
                remain = contentLimit;
            } else {
                length += take;
                remain = contentLimit - length;
            }
        }
    }

    if (!result.isEmpty()) {
        int lastLineLen = 0;
        // calc last line length
        for (int i = result.size() - 1; i >= 0; --i) {
            if (result[i].lineBreak)
                break;
            lastLineLen += result[i].contentLength;
        }

        int remain = widthLimit - lastLineLen;

        int tailTextLength = 0;
        for (int i = contentIndex + 1; i < messageParts.size(); ++i) {
            tailTextLength += messageParts[i].contentLength;
        }

        if (tailTextLength != 0) {
            if (tailTextLength > remain) {
                for (int i = result.size() - 1; i >= 0; --i) {
                    if (result[i].type == FormatPart::Type::Message) {
                        result[i].lineBreak = true;
                        break;
                    }
                }
                result.append({ FormatPart::Type::Message, QString(widthLimit - tailTextLength, ' '), false });
            } else {
                result.append({ FormatPart::Type::Message, QString(remain - tailTextLength, ' '), false });
            }
        }
    }
    for (int i = contentIndex + 1; i < messageParts.size(); ++i) {
        result << messageParts[i];
    }
    return result;
}

QList<ContentPart> TextUtils::toContentParts(const QString& content) {
    static QRegularExpression re("\x1b\\[(\\d+(;\\d+)*)m");
    auto it = re.globalMatch(content);

    int lastPos = 0;
    QList<ContentPart> logPart;
    //split log text and color style code
    while (it.hasNext()) {
        auto match = it.next();
        auto pos = match.capturedStart();
        //previous string
        logPart.append(ContentPart(content.mid(lastPos, pos - lastPos), false));
        //linux style code
        logPart.append(ContentPart(match.captured(1), true));
        //process next
        lastPos = match.capturedEnd();
    }
    logPart.append(ContentPart(content.mid(lastPos), false));
    return logPart;
}

QString TextUtils::removeColors(const QString& content) {
    static QRegularExpression re("\x1b\\[(\\d+(;\\d+)*)m");
    auto stripped = content;
    stripped.remove(re);
    return stripped;
}

int TextUtils::displayWidth(const QString& content) {
    auto stripped = removeColors(content);
    int width = 0;
    auto ucs4 = stripped.toUcs4();
    for (auto ch : ucs4) {
        width += charDisplayWidth(ch);
    }
    return width;
}

int TextUtils::charDisplayWidth(uint32_t codePoint) {
    if (codePoint < 0x7F)
        return 1;

    if ((codePoint >= 0x1100 && codePoint <= 0x115F) ||
        (codePoint >= 0x2E80 && codePoint <= 0xA4CF) ||
        (codePoint >= 0xAC00 && codePoint <= 0xD7A3) ||
        (codePoint >= 0xF900 && codePoint <= 0xFAFF) ||
        (codePoint >= 0xFE10 && codePoint <= 0xFE6F) ||
        (codePoint >= 0xFF00 && codePoint <= 0xFF60) ||
        (codePoint >= 0xFFE0 && codePoint <= 0xFFE6))
        return 2;

    if (codePoint >= 0x1F300 && codePoint <= 0x1FAD6)
        return 2;

    return 1;
}

ColorFormatter TextUtils::getLevelCodeFormatter(const QString& message) {
    ColorFormatter formatter;
    if (message == "D") {
        formatter.setForeground(ColorAttr::Green, 1);
    } else if (message == "I") {
        formatter.setForeground(ColorAttr::Blue, 1);
    } else if (message == "W") {
        formatter.setForeground(ColorAttr::Yellow, 1);
    } else if (message == "E") {
        formatter.setForeground(ColorAttr::Red, 1);
    }
    return formatter;
}

ColorFormatter TextUtils::getFileCodeFormatter(const ColorFormatter* srcStyle) {
    ColorFormatter formatter;
    if (srcStyle != nullptr) {
        formatter = *srcStyle;
    }
    if (!formatter.isForegroundValid()) {
        formatter.setForeground(ColorAttr::Blue);
    }
    formatter.underline();
    return formatter;
}

QLOGCOLLECTOR_END_NAMESPACE