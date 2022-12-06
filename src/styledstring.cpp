#include "styledstring.h"

namespace logcollector {
    StyledString::StyledString(QString log, bool blink, bool underline)
        : mLog(std::move(log))
    {
        if (blink) {
            formatter.blink();
        }
        if (underline) {
            formatter.underline();
        }
    }

    StyledString &logcollector::StyledString::b(uint8_t lighter) {
        formatter.setForeground(ColorAttr::Blue, lighter);
        return *this;
    }

    StyledString &StyledString::bb(uint8_t lighter) {
        formatter.setBackground(ColorAttr::Blue, lighter);
        return *this;
    }

    StyledString &logcollector::StyledString::g(uint8_t lighter) {
        formatter.setForeground(ColorAttr::Green, lighter);
        return *this;
    }

    StyledString &StyledString::gb(uint8_t lighter) {
        formatter.setBackground(ColorAttr::Green, lighter);
        return *this;
    }

    StyledString &logcollector::StyledString::c(uint8_t lighter) {
        formatter.setForeground(ColorAttr::Cyan, lighter);
        return *this;
    }

    StyledString &StyledString::cb(uint8_t lighter) {
        formatter.setBackground(ColorAttr::Cyan, lighter);
        return *this;
    }

    StyledString &logcollector::StyledString::r(uint8_t lighter) {
        formatter.setForeground(ColorAttr::Red, lighter);
        return *this;
    }

    StyledString &StyledString::rb(uint8_t lighter) {
        formatter.setBackground(ColorAttr::Red, lighter);
        return *this;
    }

    StyledString &logcollector::StyledString::p(uint8_t lighter) {
        formatter.setForeground(ColorAttr::Purple, lighter);
        return *this;
    }

    StyledString &StyledString::pb(uint8_t lighter) {
        formatter.setBackground(ColorAttr::Purple, lighter);
        return *this;
    }

    StyledString &logcollector::StyledString::y(uint8_t lighter) {
        formatter.setForeground(ColorAttr::Yellow, lighter);
        return *this;
    }

    StyledString &StyledString::yb(uint8_t lighter) {
        formatter.setBackground(ColorAttr::Yellow, lighter);
        return *this;
    }

    StyledString &logcollector::StyledString::w(uint8_t lighter) {
        formatter.setForeground(ColorAttr::White, lighter);
        return *this;
    }

    StyledString &StyledString::wb(uint8_t lighter) {
        formatter.setBackground(ColorAttr::White, lighter);
        return *this;
    }

    QString StyledString::toStyledString() const {

        QString styled = bufferLeft;
        if (!styled.isEmpty()) {
            styled += " ";
        }

        if (formatter.isInvalid())
        {
            styled += mLog;
            if (!bufferRight.isEmpty()) {
                styled += " " + bufferRight;
            }

            return styled;
        }

        styled += formatter.toStdColorCode();
        styled += mLog;
        styled += "\033[0m";

        if (!bufferRight.isEmpty()) {
            styled += " " + bufferRight;
        }
        return styled;
    }

    StyledString &StyledString::append(const char *s) {
        bufferRight.append(s);
        return *this;
    }

    StyledString &StyledString::append(const QString &s) {
        bufferRight.append(s);
        return *this;
    }

    StyledString &StyledString::prepend(const char *s) {
        bufferLeft.prepend(s);
        return *this;
    }

    StyledString &StyledString::prepend(const QString &s) {
        bufferLeft.prepend(s);
        return *this;
    }
}

logcollector::StyledString styled(const QString& log, bool blink, bool underline) {
    return logcollector::StyledString(log, blink, underline);
}