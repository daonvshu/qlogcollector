#pragma once

#include <qobject.h>
#include <qdebug.h>

#include "message.h"
#include "colorformatter.h"
#include "consolestyle.h"

namespace logcollector {

    class Console {
    public:
        Console();

        static Console &instance();

        void printMessage(Message &message);

    private:
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

    struct ConsoleLogPart {
        QString part;
        bool isStyleCode;
        bool nextLine = false;

        int length() const {
            return part.length();
        }
    };
}