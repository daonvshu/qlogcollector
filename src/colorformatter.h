#pragma once

#include "colorattr.h"

#include <qobject.h>

namespace logcollector {

    class ColorFormatter {
    public:
        explicit ColorFormatter();

        ColorFormatter& setForeground(const ColorAttr& color, uint8_t lighter = 0);

        ColorFormatter& setBackground(const ColorAttr& color, uint8_t lighter = 0);

        ColorFormatter& blink(bool set = true);

        ColorFormatter& underline(bool set = true);

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

}