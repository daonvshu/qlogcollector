#include "colorformatter.h"
#include "outputstyleconfig.h"
#include "logcollector.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE
    ColorFormatter::ColorFormatter()
    : mBlink(false)
    , mUnderline(false)
    , foreground({ColorAttr::Unset, 0})
    , background({ColorAttr::Unset, 0})
{}

ColorFormatter &ColorFormatter::setForeground(const ColorAttr &color, uint8_t lighter) {
    foreground = qMakePair(color, lighter);
    return *this;
}

bool ColorFormatter::isForegroundValid() const {
    return foreground.first != ColorAttr::Unset;
}

ColorFormatter &ColorFormatter::setBackground(const ColorAttr &color, uint8_t lighter) {
    background = qMakePair(color, lighter);
    return *this;
}

bool ColorFormatter::isBackgroundValid() const {
    return background.first != ColorAttr::Unset;
}

ColorFormatter &ColorFormatter::blink(bool set) {
    mBlink = set;
    return *this;
}

bool ColorFormatter::isBlink() const {
    return mBlink;
}

ColorFormatter &ColorFormatter::underline(bool set) {
    mUnderline = set;
    return *this;
}

bool ColorFormatter::isUnderline() const {
    return mUnderline;
}

bool ColorFormatter::isInvalid() const {
    return !mBlink && !mUnderline && foreground.first == ColorAttr::Unset
           && background.first == ColorAttr::Unset;
}

QString ColorFormatter::toStdColorCode() const {
    QString styled = "\033[";
    if (mBlink) {
        styled += QString::number((int)StdColorAttr::LBlink);
        styled += ";";
    }
    if (mUnderline) {
        if (LogCollector::styleConfig.mUnderlineEnabled) {
            styled += QString::number((int) StdColorAttr::LU);
            styled += ";";
        }
    }

    if (foreground.first != ColorAttr::Unset) {
        int v = 30 + (int)foreground.first;
        if (foreground.second && LogCollector::styleConfig.mLighterColorEnabled) {
            v += (int)StdColorAttr::LL;
        }
        styled += QString::number(v) + ";";
    }
    if (background.first != ColorAttr::Unset) {
        int v = 30 + (int)background.first + (int)StdColorAttr::LBK;
        if (background.second && LogCollector::styleConfig.mLighterColorEnabled) {
            v += (int)StdColorAttr::LL;
        }
        styled += QString::number(v) + ";";
    }
    styled.chop(1);
    styled += "m";
    return styled;
}

int ColorFormatter::toWin32ColorCode() const {
    int styled = 0;
    if (mBlink) {
        //unsupported operator, ignore
    }
    if (mUnderline) {
        if (LogCollector::styleConfig.mUnderlineEnabled) {
            styled |= (int) Win32ColorAttr::WU;
        }
    }

    const auto colorAttrToCode = [] (const ColorAttr& attr) {
        switch (attr) {
            case ColorAttr::Red:
                return (int)Win32ColorAttr::WR;
            case ColorAttr::Green:
                return (int)Win32ColorAttr::WG;
            case ColorAttr::Yellow:
                return (int)Win32ColorAttr::WY;
            case ColorAttr::Blue:
                return (int)Win32ColorAttr::WB;
            case ColorAttr::Purple:
                return (int)Win32ColorAttr::WP;
            case ColorAttr::Cyan:
                return (int)Win32ColorAttr::WC;
            case ColorAttr::White:
                return (int)Win32ColorAttr::WW;
            case ColorAttr::Unset:
                break;
        }
        return 0;
    };

    if (foreground.first != ColorAttr::Unset) {
        int code = colorAttrToCode(foreground.first);
        if (foreground.second && LogCollector::styleConfig.mLighterColorEnabled) {
            code |= (int)Win32ColorAttr::WL;
        }
        styled |= code;
    }

    if (background.first != ColorAttr::Unset) {
        int code = colorAttrToCode(background.first);
        if (background.second && LogCollector::styleConfig.mLighterColorEnabled) {
            code |= (int)Win32ColorAttr::WL;
        }
        code = code << (int)Win32ColorAttr::WBK;
        styled |= code;
    }

    return styled;
}

ColorFormatter ColorFormatter::fromLinuxColorCode(const QList<int>& codes) {
    if (codes.isEmpty()) {
        return ColorFormatter();
    }
    ColorFormatter formatter;
    for (int c : codes) {
        if (c == 0) {
            formatter = ColorFormatter();
        } else if (c == 4) {
            formatter.underline();
        } else if (c == 5) {
            formatter.blink();
        } else {
            int tryCode = c - 30;
            if (tryCode >= (int)ColorAttr::Unset && tryCode <= (int)ColorAttr::White) {
                formatter.setForeground(ColorAttr(tryCode));
                continue;
            }

            tryCode = c - (int)StdColorAttr::LBK - 30;
            if (tryCode >= (int)ColorAttr::Unset && tryCode <= (int)ColorAttr::White) {
                formatter.setBackground(ColorAttr(tryCode));
                continue;
            }

            tryCode = c - 30 - (int)StdColorAttr::LL;
            if (tryCode >= (int)ColorAttr::Unset && tryCode <= (int)ColorAttr::White) {
                formatter.setForeground(ColorAttr(tryCode), 1);
                continue;
            }

            tryCode = c - (int)StdColorAttr::LBK - 30 - (int)StdColorAttr::LL;
            if (tryCode >= (int)ColorAttr::Unset && tryCode <= (int)ColorAttr::White) {
                formatter.setBackground(ColorAttr(tryCode), 1);
                continue;
            }
        }
    }
    return formatter;
}

QLOGCOLLECTOR_END_NAMESPACE