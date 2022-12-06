#pragma once

#include <qobject.h>
#include <qdebug.h>

#include "message.h"
#include "colorformatter.h"

namespace logcollector {

    enum class ConsoleOutputTarget {
        TARGET_STANDARD_OUTPUT,
        TARGET_WIN32_CONSOLE_APP,
        TARGET_WIN32_DEBUG_CONSOLE,
    };

    class Console {
    public:
        Console();

        static Console &instance();

        static void resetConsoleOutputTarget(const ConsoleOutputTarget &target);

        void printMessage(Message &message);

    private:
        static ConsoleOutputTarget outputTarget;

        ColorFormatter currentColorFormatter;

#if defined Q_CC_MSVC
        WORD wOldColorAttrs;
        HANDLE consoleHandle;
#endif

    private:
        void print(const QString& log);
        void beginMessageType(const QtMsgType& type);
        void endStyle();
    };

}