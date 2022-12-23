#include "console.h"

#include <iostream>
#include <qregexp.h>
#include <qdatetime.h>

namespace logcollector {

    ConsoleOutputStyleConfig styleConfig;

    Console &Console::instance() {
        static Console console;
        return console;
    }

    void Console::printMessage(Message &message) {

        if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE) {
            //add extra code line to navigate to source code line
            QString codeLine = message.fileName() + "(" + QString::number(message.codeLine()) + "):\n";
#if defined Q_OS_WIN
            //ignore unsupported color style
            OutputDebugString(reinterpret_cast<const wchar_t*>(codeLine.utf16()));
#endif
        }

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

        const int logHeaderUsedCharSize = 46;
        const auto logHeaderPlaceholder = "\n" + QString(" ").repeated(logHeaderUsedCharSize);

        QString codeLine = "(" + message.fileName() + ":" + QString::number(message.codeLine()) + ")";
        auto codeLinePrint = [&] (const QString& space = QString()) {
            print(" " + space);
            currentColorFormatter = ColorFormatter().setForeground(ColorAttr::Blue).underline();
            print(codeLine);
            endStyle();
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
                        while (logPart.at(++nextPos).isStyleCode);
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
                    }
                } else {
                    if (part.isStyleCode) {
                        print("\x1b[" + part.part + "m");
                    } else {
                        if (part.nextLine) {
                            print(logHeaderPlaceholder);
                            lastLogLength = 0;
                        }
                        print(part.part);
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
            print(content);
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

    Console::Console() {
#if defined Q_OS_WIN
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(consoleHandle, &bufferInfo);
        wOldColorAttrs = bufferInfo.wAttributes;
#endif
    }

    void Console::print(const QString &log) {
        if (log.isEmpty()) {
            return;
        }
        if (styleConfig.mOutputTarget == ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP) {
            if (!currentColorFormatter.isInvalid()) {
#if defined Q_OS_WIN
                SetConsoleTextAttribute(consoleHandle, currentColorFormatter.toWin32ColorCode());
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
                if (!currentColorFormatter.isInvalid()) {
                    auto styleCode = currentColorFormatter.toStdColorCode();
                    OutputDebugString(reinterpret_cast<const wchar_t*>(styleCode.utf16()));
                }
            }
            //ignore unsupported color style
            OutputDebugString(reinterpret_cast<const wchar_t*>(log.utf16()));
#endif
        } else {
            if (!currentColorFormatter.isInvalid()) {
                std::cout << currentColorFormatter.toStdColorCode().toStdString();
            }
            if (styleConfig.useSystemCodePage) {
                std::cout << log.toLocal8Bit().data();
            } else {
                std::cout << log.toStdString();
            }
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
