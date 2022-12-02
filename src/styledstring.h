#pragma once

#include <qobject.h>
#include <qdebug.h>

namespace logcollector {

    enum class ColorAttr {
        Unset = 0,
        Blue,
        Green,
        Cyan,
        Red,
        Purple,
        Yellow,
        White,
    };

    class StyledString {
    public:
        explicit StyledString(QString log, bool blink, bool underline);

        //blue
        StyledString& b(uint8_t lighter = 0);
        //background blue
        StyledString& bb(uint8_t lighter = 0);
        //green
        StyledString& g(uint8_t lighter = 0);
        //background green
        StyledString& gb(uint8_t lighter = 0);
        //cyan
        StyledString& c(uint8_t lighter = 0);
        //background cyan
        StyledString& cb(uint8_t lighter = 0);
        //red
        StyledString& r(uint8_t lighter = 0);
        //background red
        StyledString& rb(uint8_t lighter = 0);
        //purple
        StyledString& p(uint8_t lighter = 0);
        //background purple
        StyledString& pb(uint8_t lighter = 0);
        //yellow
        StyledString& y(uint8_t lighter = 0);
        //background yellow
        StyledString& yb(uint8_t lighter = 0);
        //white
        StyledString& w(uint8_t lighter = 0);
        //background white
        StyledString& wb(uint8_t lighter = 0);

        QString toStyledString() const;

    private:
        QString mLog;
        bool mBlink;
        bool mUnderline;

        QPair<ColorAttr, uint8_t> foreground, background;

        QString bufferLeft, bufferRight;

        friend inline StyledString& operator+(const char* s, StyledString& styledString);
        friend inline StyledString& operator+(const QString& s, StyledString& styledString);
        friend inline StyledString& operator+(StyledString& styledString, const char* s);
        friend inline StyledString& operator+(StyledString& styledString, const QString& s);

    private:
        StyledString& append(const char* s);
        StyledString& append(const QString& s);
        StyledString& prepend(const char* s);
        StyledString& prepend(const QString& s);
    };

    inline StyledString& operator+(const char* s, StyledString& styledString) {
        return styledString.prepend(s);
    }

    inline StyledString& operator+(const QString& s, StyledString& styledString) {
        return styledString.prepend(s);
    }

    inline StyledString& operator+(StyledString& styledString, const char* s) {
        return styledString.append(s);
    }

    inline StyledString& operator+(StyledString& styledString, const QString& s) {
        return styledString.append(s);
    }
}

extern logcollector::StyledString styled(const QString& log, bool blink = false, bool underline = false);

inline QDebug operator<<(QDebug debug, const logcollector::StyledString& styledString) {
    QDebugStateSaver saver(debug);
    debug.noquote() << styledString.toStyledString();
    return debug;
}
