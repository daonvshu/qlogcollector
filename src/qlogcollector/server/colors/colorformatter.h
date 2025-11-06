#pragma once

#include "colorattr.h"

#include <qobject.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT ColorFormatter {
public:
    explicit ColorFormatter();

    ColorFormatter& setForeground(const ColorAttr& color, uint8_t lighter = 0);

    bool isForegroundValid() const;

    ColorFormatter& setBackground(const ColorAttr& color, uint8_t lighter = 0);

    bool isBackgroundValid() const;

    ColorFormatter& blink(bool set = true);

    bool isBlink() const;

    ColorFormatter& underline(bool set = true);

    bool isUnderline() const;

    bool isInvalid() const;

    QString toStdColorCode() const;

    int toWin32ColorCode() const;

    static ColorFormatter fromLinuxColorCode(const QList<int>& codes);

private:
    bool mBlink;
    bool mUnderline;

    QPair<ColorAttr, uint8_t> foreground, background;

    friend class StyledString;
};

QLOGCOLLECTOR_END_NAMESPACE