#pragma once

#include <qobject.h>

#include <qlogcollector/comm/global.h>
#include <qlogcollector/server/colors/colorformatter.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

struct FormatPart {
    enum class Type {
        Literal,
        Date,
        Level,
        Category,
        Message,
        Thread,
        File,
    };

    Type type;
    QString content;
    int contentLength = 0;
    bool lineBreak = false;
    ColorFormatter* colorFormatter = nullptr;

    FormatPart(Type type, const QString& content, bool lineBreak = false, ColorFormatter* colorFormatter = nullptr);
};

struct ContentPart {
    QString part;
    bool isStyleCode;

    ContentPart(const QString& part, bool isStyleCode)
        : part(part), isStyleCode(isStyleCode)
    {}

    int length() const {
        return part.length();
    }
};

class QLOGCOLLECTOR_EXPORT TextUtils {
public:
    static QList<FormatPart> wordwrap(const QList<FormatPart>& messageParts);

    static QList<ContentPart> toContentParts(const QString& content);
    static QString removeColors(const QString& content);
    static int displayWidth(const QString& content);
    static int charDisplayWidth(uint32_t codePoint);

    static ColorFormatter getLevelCodeFormatter(const QString& message);
    static ColorFormatter getFileCodeFormatter(const ColorFormatter* srcStyle);
};

QLOGCOLLECTOR_END_NAMESPACE