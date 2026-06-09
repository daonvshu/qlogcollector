#include "patternformatter.h"
#include "outputtarget.h"

#include <qdatetime.h>
#include <qregularexpression.h>
#include <qsharedpointer.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

FormatPart::FormatPart(Type type, const QString& content, bool lineBreak, ColorFormatter* colorFormatter)
    : type(type), content(content), lineBreak(lineBreak), colorFormatter(colorFormatter)
{
    contentLength = TextUtils::removeColors(content).length(); //TextUtils::displayWidth(content);
}

//--------------------------------------------
// LiteralToken
//--------------------------------------------
FormatPart PatternFormatter::LiteralToken::format(const Message&) {
    return FormatPart(FormatPart::Type::Literal, m_literal, false, &m_style);
}

//--------------------------------------------
// DateToken
//--------------------------------------------
PatternFormatter::DateToken::DateToken(QString fmt)
    : m_fmt(std::move(fmt))
{
    if (m_fmt.isEmpty())
        m_fmt = "hh:mm:ss.zzz";
}

FormatPart PatternFormatter::DateToken::format(const Message& msg)
{
    auto dt = QDateTime::fromMSecsSinceEpoch(msg.timePoint);
    return FormatPart(FormatPart::Type::Date, dt.toString(m_fmt), false, &m_style);
}

//--------------------------------------------
// LevelToken
//--------------------------------------------
FormatPart PatternFormatter::LevelToken::format(const Message& msg)
{
    static char levels [] = "DWEEI";
    QString content;
    if (msg.level < 0 || msg.level > 4) {
        content = QString::number(msg.level);
    } else {
        content = QString(levels[msg.level]);
    }
    return FormatPart(FormatPart::Type::Level, content);
}

//--------------------------------------------
// CategoryToken
//--------------------------------------------
FormatPart PatternFormatter::CategoryToken::format(const Message& msg) {
    m_minWidth = qMax(m_minWidth, msg.category.size());
    return FormatPart(FormatPart::Type::Category, msg.category.leftJustified(m_minWidth), false, &m_style);
}

//--------------------------------------------
// MessageToken
//--------------------------------------------
FormatPart PatternFormatter::MessageToken::format(const Message& msg) {
    return FormatPart(FormatPart::Type::Message, msg.log);
}

//--------------------------------------------
// ThreadToken
//--------------------------------------------
FormatPart PatternFormatter::ThreadToken::format(const Message& msg) {
    QString threadName = msg.threadName;
    if (threadName.isEmpty()) {
        threadName = QStringLiteral("T") + QString::number(static_cast<quintptr>(msg.threadId), 16);
    }
    m_minWidth = qMax(m_minWidth, threadName.size());
    return FormatPart(FormatPart::Type::Thread, threadName.leftJustified(m_minWidth), false, &m_style);
}

//--------------------------------------------
// FileToken
//--------------------------------------------
FormatPart PatternFormatter::FileToken::format(const Message& msg) {
    QString content = msg.fileName + ":" + QString::number(msg.codeLine);
    //m_minWidth = qMax(m_minWidth, content.size());
    return FormatPart(FormatPart::Type::File, content, false, &m_style);
}

PatternFormatter::PatternFormatter(QObject* parent)
    : QObject(parent) {
}

void PatternFormatter::setFormat(const QString& pattern) {
    m_tokens.clear();

    QRegularExpression re("%(\\w)(?:\\{([^}]*)\\})?(?:\\[([^]]*)\\])?");
    int lastPos = 0;
    auto it = re.globalMatch(pattern);
    while (it.hasNext()) {
        auto match = it.next();
        int start = match.capturedStart();
        if (start > lastPos) {
            QString literal = pattern.mid(lastPos, start - lastPos);
            m_tokens << QSharedPointer<LiteralToken>::create(literal);
        }

        QString code = match.captured(1);
        QString arg  = match.captured(2);
        QString styleArg = match.captured(3);

        ColorFormatter style;
        if (!styleArg.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            const QStringList kvs = styleArg.split(',', Qt::SkipEmptyParts);
#else
            const QStringList kvs = styleArg.split(',', QString::SkipEmptyParts);
#endif
            for (const QString& kv : kvs) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                const QStringList pair = kv.split('=', Qt::KeepEmptyParts);
#else
                const QStringList pair = kv.split('=', QString::KeepEmptyParts);
#endif
                QString key = pair.value(0).trimmed().toLower();
                QString val = pair.value(1).trimmed().toLower();

                if (key == "fg") style.setForeground(getColorAttr(val));
                else if (key == "bg") style.setBackground(getColorAttr(val));
                else if (key == "bl") style.blink();
                else if (key == "ul") style.underline();
            }
        }

        QSharedPointer<PatternToken> token;
        if (code == "d")      token = QSharedPointer<DateToken>::create(arg);
        else if (code == "p") token = QSharedPointer<LevelToken>::create();
        else if (code == "c") token = QSharedPointer<CategoryToken>::create();
        else if (code == "m") token = QSharedPointer<MessageToken>::create();
        else if (code == "t") token = QSharedPointer<ThreadToken>::create();
        else if (code == "f") token = QSharedPointer<FileToken>::create();
        else {
            token = QSharedPointer<LiteralToken>::create("%" + code);
        }
        token->setStyle(style);
        m_tokens << token;

        lastPos = match.capturedEnd();
    }

    if (lastPos < pattern.size()) {
        m_tokens << QSharedPointer<LiteralToken>::create(pattern.mid(lastPos));
    }
}

QList<FormatPart> PatternFormatter::format(const Message& msg) const {
    QList<FormatPart> result;
    for (auto& token : m_tokens) {
        result << token->format(msg);
    }
    return result;
}

ColorAttr PatternFormatter::getColorAttr(const QString& name) {
    if (name == "r") {
        return ColorAttr::Red;
    }
    if (name == "g") {
        return ColorAttr::Green;
    }
    if (name == "y") {
        return ColorAttr::Yellow;
    }
    if (name == "b") {
        return ColorAttr::Blue;
    }
    if (name == "p") {
        return ColorAttr::Purple;
    }
    if (name == "c") {
        return ColorAttr::Cyan;
    }
    if (name == "w") {
        return ColorAttr::White;
    }
    return ColorAttr::Unset;
}

QLOGCOLLECTOR_END_NAMESPACE
