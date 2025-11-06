#pragma once

#include <qobject.h>

#include <qlogcollector/comm/global.h>
#include <qlogcollector/comm/message.h>
#include <qlogcollector/server/utils/textutils.h>
#include <qlogcollector/server/colors/colorformatter.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

class PatternFormatter : public QObject {
public:
    explicit PatternFormatter(QObject* parent = nullptr);

    void setFormat(const QString& pattern);

    QList<FormatPart> format(const Message& msg) const;

private:
    static ColorAttr getColorAttr(const QString& name);

private:
    class PatternToken {
    public:
        virtual ~PatternToken() = default;
        virtual FormatPart format(const Message& msg) = 0;

        void setStyle(const ColorFormatter& style) {
            m_style = style;
        }

    protected:
        ColorFormatter m_style;
        int m_minWidth = 0;
    };

    class LiteralToken final : public PatternToken {
    public:
        explicit LiteralToken(QString literal) : m_literal(std::move(literal)) {}
        FormatPart format(const Message&) override;

    private:
        QString m_literal;
    };

    class DateToken final : public PatternToken {
    public:
        explicit DateToken(QString fmt);
        FormatPart format(const Message& msg) override;

    private:
        QString m_fmt;
    };

    class LevelToken final : public PatternToken {
    public:
        FormatPart format(const Message& msg) override;
    };

    class CategoryToken final : public PatternToken {
    public:
        FormatPart format(const Message& msg) override;
    };

    class MessageToken final : public PatternToken {
    public:
        FormatPart format(const Message& msg) override;
    };

    class ThreadToken final : public PatternToken {
    public:
        FormatPart format(const Message& msg) override;
    };

    class FileToken final : public PatternToken {
    public:
        FormatPart format(const Message& msg) override;
    };

    QList<QSharedPointer<PatternToken>> m_tokens;
};

QLOGCOLLECTOR_END_NAMESPACE