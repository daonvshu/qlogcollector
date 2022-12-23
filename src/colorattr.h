#pragma once

#include <qobject.h>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

namespace logcollector {

    //win32 console color attribute
    enum class Win32ColorAttr {
#ifdef Q_OS_WIN
        WR = FOREGROUND_RED,
        WG = FOREGROUND_GREEN,
        WB = FOREGROUND_BLUE,

        //mix
        WY = (WR | WG),
        WP = (WR | WB),
        WC = (WG | WB),
        WW = (WR | WG | WB),

        //background
        WBK = 4, // x << WBK

        //lighter
        WL = FOREGROUND_INTENSITY, // x |= WL

        //underline
        WU = COMMON_LVB_UNDERSCORE,
#else
        WR,
        WG,
        WB,
        WY,
        WP,
        WC,
        WW,
        WBK,
        WL,
        WU,
#endif
    };

    //linux console color attribute
    enum class StdColorAttr {
        LR = 31,
        LG = 32,
        LY = 33,
        LB = 34,
        LP = 35,
        LC = 36,
        LW = 37,

        //background
        LBK = 10, // x += LBK

        //lighter
        LL = 60, // x += LL

        //underline
        LU = 4,

        //blink
        LBlink = 5,
    };

    enum class ColorAttr {
        Unset = 0,
        Red,
        Green,
        Yellow,
        Blue,
        Purple,
        Cyan,
        White,
    };
}