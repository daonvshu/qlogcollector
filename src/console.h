#pragma once

#include <qobject.h>
#include <qdebug.h>

#include "message.h"
#include "colorformatter.h"
#include "consolestyle.h"

namespace logcollector {

    struct Console {
        static void printMessage(Message &message);
        static void print(const ColorFormatter& formatter, const QString& log);
        static ColorFormatter beginMessageType(const QtMsgType& type);
        static void endStyle();
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