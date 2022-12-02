#include "styledstring.h"

#include <utility>

namespace logcollector {
    StyledString::StyledString(QString log, bool blink, bool underline)
        : mLog(std::move(log))
        , mBlink(blink)
        , mUnderline(underline)
        , foreground{ColorAttr::Unset, 0}
        , background{ColorAttr::Unset, 0}
    {}

    StyledString &logcollector::StyledString::b(uint8_t lighter) {
        foreground = {ColorAttr::Blue, lighter};
        return *this;
    }

    StyledString &StyledString::bb(uint8_t lighter) {
        background = {ColorAttr::Blue, lighter};
        return *this;
    }

    StyledString &logcollector::StyledString::g(uint8_t lighter) {
        foreground = {ColorAttr::Green, lighter};
        return *this;
    }

    StyledString &StyledString::gb(uint8_t lighter) {
        background = {ColorAttr::Green, lighter};
        return *this;
    }

    StyledString &logcollector::StyledString::c(uint8_t lighter) {
        foreground = {ColorAttr::Cyan, lighter};
        return *this;
    }

    StyledString &StyledString::cb(uint8_t lighter) {
        background = {ColorAttr::Cyan, lighter};
        return *this;
    }

    StyledString &logcollector::StyledString::r(uint8_t lighter) {
        foreground = {ColorAttr::Red, lighter};
        return *this;
    }

    StyledString &StyledString::rb(uint8_t lighter) {
        background = {ColorAttr::Red, lighter};
        return *this;
    }

    StyledString &logcollector::StyledString::p(uint8_t lighter) {
        foreground = {ColorAttr::Purple, lighter};
        return *this;
    }

    StyledString &StyledString::pb(uint8_t lighter) {
        background = {ColorAttr::Purple, lighter};
        return *this;
    }

    StyledString &logcollector::StyledString::y(uint8_t lighter) {
        foreground = {ColorAttr::Yellow, lighter};
        return *this;
    }

    StyledString &StyledString::yb(uint8_t lighter) {
        background = {ColorAttr::Yellow, lighter};
        return *this;
    }

    StyledString &logcollector::StyledString::w(uint8_t lighter) {
        foreground = {ColorAttr::White, lighter};
        return *this;
    }

    StyledString &StyledString::wb(uint8_t lighter) {
        background = {ColorAttr::White, lighter};
        return *this;
    }

    static const int colorValues[] = {30, 34, 32, 36, 31, 35, 33, 37};
    QString StyledString::toStyledString() const {

        QString styled = bufferLeft;
        if (!styled.isEmpty()) {
            styled += " ";
        }

        if (!mBlink && !mUnderline && foreground.first == ColorAttr::Unset
            && background.first == ColorAttr::Unset)
        {
            styled += mLog;
            if (!bufferRight.isEmpty()) {
                styled += " " + bufferRight;
            }

            return styled;
        }

        styled +="\033[";
        if (mBlink) {
            styled += "5;";
        }
        if (mUnderline) {
            styled += "4;";
        }

        if (foreground.first != ColorAttr::Unset) {
            int v = colorValues[(int)foreground.first];
            if (foreground.second) {
                v += 60;
            }
            styled += QString::number(v) + ";";
        }
        if (background.first != ColorAttr::Unset) {
            int v = colorValues[(int)background.first] + 10;
            if (background.second) {
                v += 60;
            }
            styled += QString::number(v) + ";";
        }
        styled.chop(1);
        styled += "m";
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