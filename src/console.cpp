#include "console.h"

#include <iostream>
#include <qregexp.h>
#include <qdatetime.h>

namespace logcollector {

    ConsoleOutputStyleConfig styleConfig;

#if defined Q_OS_WIN
    WORD wOldColorAttrs;
    HANDLE consoleHandle;

    int rst = [] {
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(consoleHandle, &bufferInfo);
        wOldColorAttrs = bufferInfo.wAttributes;
        return 1;
    } ();
#endif

    void Console::printMessage(Message &message) {

        ColorFormatter currentColorFormatter;

        if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
            //add extra code line to navigate to source code line
            QString codeLine = message.fileName() + "(" + QString::number(message.codeLine()) + "):\n";
#if defined Q_OS_WIN
            //ignore unsupported color style
            OutputDebugString(reinterpret_cast<const wchar_t*>(codeLine.utf16()));
#endif
        }

        auto dateTime = QDateTime::fromMSecsSinceEpoch(message.timePoint());
        print(currentColorFormatter, dateTime.toString("HH:mm:ss.zzz  "));

        auto threadName = message.threadName();
        if (threadName.isEmpty()) {
            threadName = QString::number(message.threadId()).right(6);
        }
        print(currentColorFormatter, threadName.leftJustified(8, ' ', true));
        print(currentColorFormatter, "  ");

        if (message.category() == "default") {
            print(currentColorFormatter, QString("<no-category>").leftJustified(14, ' ', true));
        } else {
            print(currentColorFormatter, message.category().leftJustified(14, ' ', true));
        }
        print(currentColorFormatter, "  ");

        auto msgType = (QtMsgType)message.level();
        currentColorFormatter = beginMessageType(msgType);

        switch (msgType) {
            case QtDebugMsg:
                print(currentColorFormatter, " D-> ");
                break;
            case QtWarningMsg:
                print(currentColorFormatter, " W-> ");
                break;
            case QtInfoMsg:
                print(currentColorFormatter, " I-> ");
                break;
            case QtCriticalMsg:
            case QtFatalMsg:
                print(currentColorFormatter, " E-> ");
                break;
        }
        endStyle();
        currentColorFormatter = ColorFormatter();
        print(currentColorFormatter, " ");

        const int logHeaderUsedCharSize = 46;
        const auto logHeaderPlaceholder = "\n" + QString(" ").repeated(logHeaderUsedCharSize);

        QString codeLine = "(" + message.fileName() + ":" + QString::number(message.codeLine()) + ")";
        auto codeLinePrint = [&] (const QString& space = QString()) {
            print(currentColorFormatter, " " + space);
            currentColorFormatter = ColorFormatter().setForeground(ColorAttr::Blue).underline();
            print(currentColorFormatter, codeLine);
            endStyle();
            currentColorFormatter = ColorFormatter();
        };

        bool wordsWrapMode = styleConfig.mLogLineWidth > 0;
        auto content = message.log();
        if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP || wordsWrapMode) {
            static QRegExp rx("\x1b\\[(\\d+(;\\d+)*)m");
            int pos = 0;
            int lastPos = 0;
            QList<ConsoleLogPart> logPart;
            //split log text and color style code
            while ((pos = rx.indexIn(content, pos)) != -1) {
                //previous string
                logPart.append(ConsoleLogPart(content.mid(lastPos, pos - lastPos), false));
                //linux style code
                logPart.append(ConsoleLogPart(rx.cap(1), true));
                //process next
                pos += rx.matchedLength();
                lastPos = pos;
            }
            logPart.append(ConsoleLogPart(content.mid(lastPos), false));

            //log width limit
            if (wordsWrapMode) {
                //split text part
                int logPartIndex = 0;
                int count = 0;
                while (logPartIndex < logPart.size()) {
                    if (logPart.at(logPartIndex).isStyleCode) {
                        logPartIndex++;
                        continue;
                    }
                    count += logPart.at(logPartIndex).length();
                    if (count == styleConfig.mLogLineWidth) {
                        int nextPos = logPartIndex;
                        while (++nextPos < logPart.size() && logPart.at(nextPos).isStyleCode);
                        if (nextPos < logPart.size()) {
                            logPart[nextPos].nextLine = true;
                        }
                        logPartIndex = nextPos;
                        count = 0;
                        continue;
                    }
                    if (count > styleConfig.mLogLineWidth) {
                        //split
                        int spareLength = count - styleConfig.mLogLineWidth;
                        int index = logPart.at(logPartIndex).length() - spareLength;
                        static QRegExp rx2("[^0-9a-zA-Z\"]");
                        auto newIndex = logPart.at(logPartIndex).part.lastIndexOf(rx2, index);
                        if (newIndex != -1) {
                            if (index != newIndex) {
                                index = newIndex + 1; //split from words start
                            }
                        } else {
                            int previousLogIndex = logPartIndex;
                            while (--previousLogIndex >= 0) {
                                if (!logPart[previousLogIndex].isStyleCode) {
                                    break;
                                }
                            }
                            if (previousLogIndex >= 0) { //current log is not first log part
                                if (!logPart[logPartIndex].nextLine) {
                                    logPart[logPartIndex].nextLine = true;
                                    count = 0;
                                    continue;
                                }
                            }
                            //force split log
                        }
                        auto currentLogText = logPart.at(logPartIndex).part;
                        logPart[logPartIndex].part = currentLogText.mid(0, index);
                        //create new log part
                        logPart.insert(++logPartIndex, ConsoleLogPart(currentLogText.mid(index), false, true));
                        count = 0;
                        continue;
                    }
                    logPartIndex++;
                }
            }

            int lastLogLength = 0;
            for (const auto& part : logPart) {
                if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP && part.isStyleCode) {
                    auto codeStr = part.part.split(";");
                    QList<int> codes;
                    for (const auto& c : codeStr) {
                        codes << c.toInt();
                    }
                    bool previousValid = !currentColorFormatter.isInvalid();
                    currentColorFormatter = ColorFormatter::fromLinuxColorCode(codes);
                    if (previousValid && currentColorFormatter.isInvalid()) {
                        endStyle();
                        currentColorFormatter = ColorFormatter();
                    }
                } else {
                    if (part.isStyleCode) {
                        print(currentColorFormatter, "\x1b[" + part.part + "m");
                    } else {
                        if (part.nextLine) {
                            print(currentColorFormatter, logHeaderPlaceholder);
                            lastLogLength = 0;
                        }
                        print(currentColorFormatter, part.part);
                        lastLogLength += part.length();
                    }
                }
            }

            if (styleConfig.mOutputTarget != ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
                if (!wordsWrapMode) {
                    codeLinePrint();
                } else {
                    int spareLength = styleConfig.mLogLineWidth - (lastLogLength + codeLine.length() + 1);
                    QString space;
                    if (spareLength >= 0) {
                        space = QString(' ').repeated(spareLength);
                    } else {
                        space = logHeaderPlaceholder +
                                QString(' ').repeated(styleConfig.mLogLineWidth - codeLine.length());
                    }
                    codeLinePrint(space);
                }
            }
        } else {
            print(currentColorFormatter, content);
            if (styleConfig.mOutputTarget != ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
                codeLinePrint();
            }
        }

        if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
#if defined Q_OS_WIN
            OutputDebugString(L"\n");
#endif
        } else {
            std::cout << std::endl;
        }
    }

    void Console::print(const ColorFormatter& formatter, const QString &log) {
        if (log.isEmpty()) {
            return;
        }
        if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP) {
            if (!formatter.isInvalid()) {
#if defined Q_OS_WIN
                SetConsoleTextAttribute(consoleHandle, formatter.toWin32ColorCode());
#endif
            }
            if (styleConfig.useSystemCodePage) {
                std::cout << log.toLocal8Bit().data();
            } else {
                std::cout << log.toStdString();
            }
        } else if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
#if defined Q_OS_WIN
            if (styleConfig.win32DebugConsoleWithStdColorStyle) { //force use std color style
                if (!formatter.isInvalid()) {
                    auto styleCode = formatter.toStdColorCode();
                    OutputDebugString(reinterpret_cast<const wchar_t*>(styleCode.utf16()));
                }
            }
            //ignore unsupported color style
            OutputDebugString(reinterpret_cast<const wchar_t*>(log.utf16()));
#endif
        } else {
            if (!formatter.isInvalid()) {
                std::cout << formatter.toStdColorCode().toStdString();
            }
            if (styleConfig.useSystemCodePage) {
                std::cout << log.toLocal8Bit().data();
            } else {
                std::cout << log.toStdString();
            }
        }
    }

    ColorFormatter Console::beginMessageType(const QtMsgType& type) {
        ColorFormatter currentColorFormatter;
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
        return currentColorFormatter;
    }

    void Console::endStyle() {
        if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP) {
#if defined Q_OS_WIN
            SetConsoleTextAttribute(consoleHandle, wOldColorAttrs);
#endif
        } else if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_STANDARD_OUTPUT) {
            std::cout << "\033[0m";
        } else {
#if defined Q_OS_WIN
            if (styleConfig.win32DebugConsoleWithStdColorStyle) { //force use std color style
                OutputDebugString(L"\033[0m");
            }
#endif
        }
        //ignore unsupported color style for TARGET_WIN32_DEBUG_CONSOLE
    }
}
