#include "win32consoleapptarget.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <iostream>

QLOGCOLLECTOR_BEGIN_NAMESPACE
void Win32ConsoleAppTarget::writePart(const QList<FormatPart>& messageParts, const Message&) {

#ifdef Q_OS_WIN
    static HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    static WORD wOldColorAttrs = [] {
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        GetConsoleScreenBufferInfo(consoleHandle, &bufferInfo);
        return bufferInfo.wAttributes;
    } ();
#endif

    auto printStyle = [&] (const ColorFormatter& style) {
#ifdef Q_OS_WIN
        SetConsoleTextAttribute(consoleHandle, style.toWin32ColorCode());
#endif
    };

    auto endStyle = [&] {
#ifdef Q_OS_WIN
        SetConsoleTextAttribute(consoleHandle, wOldColorAttrs);
#endif
    };

    auto printContent = [&] (const QString& content) {
        if (isSystemCodePage()) {
            std::cout << content.toLocal8Bit().data();
        } else {
            std::cout << content.toStdString();
        }
    };

    auto parts = TextUtils::wordwrap(messageParts);
    for (auto& part : parts) {
        auto content = part.content;
        if (part.type == FormatPart::Type::Level) {
            auto style = TextUtils::getLevelCodeFormatter(content);
            printStyle(style);
            printContent(content);
            endStyle();
            continue;
        }

        if (part.type == FormatPart::Type::File) {
            auto style = TextUtils::getFileCodeFormatter(part.colorFormatter);
            printStyle(style);
            printContent(content);
            endStyle();
            continue;
        }

        if (part.type != FormatPart::Type::Message) {
            if (part.colorFormatter != nullptr && !part.colorFormatter->isInvalid()) {
                printStyle(*part.colorFormatter);
                printContent(content);
                endStyle();
            } else {
                printContent(content);
            }
            continue;
        }

        auto contentParts = TextUtils::toContentParts(content);
        bool lastStyleValid = false;
        for (auto& contentPart : contentParts) {
            if (contentPart.isStyleCode) {
                auto codeStr = contentPart.part.split(";");
                QList<int> codes;
                for (const auto& c : codeStr) {
                    codes << c.toInt();
                }
                auto style = ColorFormatter::fromLinuxColorCode(codes);
                if (style.isInvalid()) {
                    if (lastStyleValid) {
                        endStyle();
                    }
                    lastStyleValid = false;
                } else {
                    printStyle(style);
                    lastStyleValid = true;
                }
                continue;
            }
            printContent(contentPart.part);
        }
        if (lastStyleValid) {
            endStyle();
        }
        if (part.lineBreak) {
            std::cout << std::endl;
        }
    }
    if (!parts.isEmpty() && !parts.last().lineBreak) {
        std::cout << std::endl;
    }
}

QLOGCOLLECTOR_END_NAMESPACE
