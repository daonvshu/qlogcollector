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

#if defined Q_OS_WIN
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
        bool nextLine;

        ConsoleLogPart(const QString& part, bool isStyleCode, bool nextLine = false)
            : part(part), isStyleCode(isStyleCode), nextLine(nextLine)
        {}

        int length() const {
            return part.length();
        }
    };
}