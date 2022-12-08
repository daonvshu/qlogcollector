#include "console.h"

#include <iostream>
#include <qregexp.h>
#include <qdatetime.h>

namespace logcollector {

    Console &Console::instance() {
        static Console console;
        return console;
    }

    void Console::printMessage(Message &message) {
        auto dateTime = QDateTime::fromMSecsSinceEpoch(message.timePoint());
        print(dateTime.toString("HH:mm:ss.zzz  "));

        auto threadName = message.threadName();
        if (threadName.isEmpty()) {
            threadName = QString::number(message.threadId()).right(6);
        }
        print(threadName.leftJustified(8, ' ', true));
        print("  ");

        if (message.category() == "default") {
            print(QString("<no-category>").leftJustified(14, ' ', true));
        } else {
            print(message.category().leftJustified(14, ' ', true));
        }
        print("  ");

        auto msgType = (QtMsgType)message.level();
        beginMessageType(msgType);

        switch (msgType) {
            case QtDebugMsg:
                print(" D-> ");
                break;
            case QtWarningMsg:
                print(" W-> ");
                break;
            case QtInfoMsg:
                print(" I-> ");
                break;
            case QtCriticalMsg:
            case QtFatalMsg:
                print(" E-> ");
                break;
        }
        endStyle();
        print(" ");

        auto content = message.log();
        if (outputTarget == ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP) {
            //linux color code to win32 console code
            static QRegExp rx("\x1b\\[(\\d+(;\\d+)*)m");
            int pos = 0;
            int lastPos = 0;
            while ((pos = rx.indexIn(content, pos)) != -1) {
                //print previous string
                print(content.mid(lastPos, pos - lastPos));
                //translate linux console code to win32 code
                auto colorCode = rx.cap(1);
                auto codeStr = colorCode.split(";");
                QList<int> codes;
                for (const auto& c : codeStr) {
                    codes << c.toInt();
                }
                bool previousValid = !currentColorFormatter.isInvalid();
                currentColorFormatter = ColorFormatter::fromLinuxColorCode(codes);
                if (previousValid && currentColorFormatter.isInvalid()) {
                    endStyle();
                }

                pos += rx.matchedLength();
                lastPos = pos;
            }
            print(content.mid(lastPos));
        } else {
            print(content);
        }

        print(" (");
        currentColorFormatter = ColorFormatter().setForeground(ColorAttr::Blue).underline();

        QString line = message.fileName() + ":" + QString::number(message.codeLine());
        print(line);
        endStyle();
        print(")");

        if (outputTarget == ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
#if defined Q_CC_MSVC
            OutputDebugString(L"\n");
#endif
        } else {
            std::cout << std::endl;
        }
    }

    ConsoleOutputTarget Console::outputTarget = ConsoleOutputTarget::TARGET_STANDARD_OUTPUT;

    Console::Console() {
#if defined Q_CC_MSVC
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(consoleHandle, &bufferInfo);
        wOldColorAttrs = bufferInfo.wAttributes;
#endif
    }

    void Console::resetConsoleOutputTarget(const ConsoleOutputTarget &target) {
        outputTarget = target;
    }

    void Console::print(const QString &log) {
        if (outputTarget == ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP) {
            if (!currentColorFormatter.isInvalid()) {
#if defined Q_CC_MSVC
                SetConsoleTextAttribute(consoleHandle, currentColorFormatter.toWin32ColorCode());
#endif
            }
            std::cout << log.toStdString();
        } else if (outputTarget == ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
#if defined Q_CC_MSVC
            //ignore unsupported color style
            OutputDebugString(reinterpret_cast<const wchar_t*>(log.utf16()));
#endif
        } else {
            if (!currentColorFormatter.isInvalid()) {
                std::cout << currentColorFormatter.toStdColorCode().toStdString();
            }
            std::cout << log.toStdString();
        }
    }

    void Console::beginMessageType(const QtMsgType& type) {
        currentColorFormatter = ColorFormatter();
        switch (type) {
            case QtDebugMsg:
                currentColorFormatter.setForeground(ColorAttr::Green, 1);
                break;
            case QtInfoMsg:
                currentColorFormatter.setForeground(ColorAttr::Blue, 1);
                break;
            case QtWarningMsg:
                currentColorFormatter.setForeground(ColorAttr::Yellow, 1);
                break;
            case QtCriticalMsg:
            case QtFatalMsg:
                currentColorFormatter.setForeground(ColorAttr::Red, 1);
                break;
        }
    }

    void Console::endStyle() {
        currentColorFormatter = ColorFormatter();
        if (outputTarget == ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP) {
#if defined Q_CC_MSVC
            SetConsoleTextAttribute(consoleHandle, wOldColorAttrs);
#endif
        } else if (outputTarget == ConsoleOutputTarget::TARGET_STANDARD_OUTPUT) {
            std::cout << "\033[0m";
        }
        //ignore unsupported color style for TARGET_WIN32_DEBUG_CONSOLE
    }
}
