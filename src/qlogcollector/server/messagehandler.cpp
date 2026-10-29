#include "messagehandler.h"

#include <logcollector.h>
#include <qregularexpression.h>

#include "outputstyleconfig.h"
#include "printmarker.h"
#include "utils/textutils.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

//Markers written by the print helpers are invisible characters in front of the log
//statement, QDebug puts a separator space after each of them.
static bool isPrintMarker(const QChar& ch) {
    return ch == allowNonAsciiPrintMarker()
        || ch == noWrapPrintMarker()
        || ch == logThrottleMarker();
}

MessageHandler::MessageHandler(QObject* parent)
    : QThread(parent)
{
    setMessageFormat("[%d{HH:mm:ss.zzz}[fg=g]] [%t] %c %p-> %m (%f)");
    //mark the suppressed count appended to a throttled line, same red as level e
    throttleNoteStyle.setForeground(ColorAttr::Red, 1);
}

void MessageHandler::addOutputTarget(OutputTarget* outputTarget) {
    outputTargets << QSharedPointer<OutputTarget>(outputTarget);
    if (outputTarget->enableTraceCollection()) {
        traceCollectionEnabled.storeRelease(1);
    }
}

void MessageHandler::setMessageFormat(const QString& format) {
    formatter.setFormat(format);
}

void MessageHandler::processMessage(const Message& message) {
    QMutexLocker locker(&messageMutex);

    Message filterMsg = message;
    bool allowNonAscii = false;
    bool noWrap = message.noWrap;
    bool throttled = message.throttled;
    //consume every marker in front of the content, they can be combined in any order
    while (!filterMsg.log.isEmpty() && isPrintMarker(filterMsg.log.at(0))) {
        const QChar marker = filterMsg.log.at(0);
        if (marker == allowNonAsciiPrintMarker()) {
            allowNonAscii = true;
        } else if (marker == noWrapPrintMarker()) {
            noWrap = true;
        } else {
            throttled = true;
        }
        filterMsg.log.remove(0, 1);
        if (filterMsg.log.startsWith(QLatin1Char(' '))) {
            filterMsg.log.remove(0, 1);
        }
    }
    filterMsg.noWrap = noWrap;
    filterMsg.throttled = throttled;

    if (LogCollector::styleConfig.mNonAsciiCheckEnabled && !allowNonAscii) {
        filterMsg.log = sanitizeLogMessage(filterMsg.log);
    }
    messages.append(filterMsg);
    messageCondition.notify_all();
}

void MessageHandler::flush() {
    forceFlush.storeRelease(true);
    messageCondition.notify_all();
}

void MessageHandler::exit() {
    QMutexLocker locker(&messageMutex);
    requestInterruption();
    messageCondition.notify_all();
}

#define AUTO_FLUSH_INTERVAL 1000
void MessageHandler::run() {
    while (true) {
        messageMutex.lock();
        if (messages.isEmpty()) {
            if (isInterruptionRequested() || forceFlush.loadAcquire()) {
                if (forceFlush.loadAcquire()) {
                    auto buffer = messages;
                    messages.clear();
                    messageMutex.unlock();
                    writeMessage(buffer);
                    flushAllTargets(true);
                } else {
                    messageMutex.unlock();
                }
                break;
            }
            if (!messageCondition.wait(&messageMutex, AUTO_FLUSH_INTERVAL)) {
                flushAllTargets(false);
                messageMutex.unlock();
                continue;
            }
        }
        if (messages.isEmpty()) {
            messageMutex.unlock();
            continue;
        }
        auto buffer = messages;
        messages.clear();
        messageMutex.unlock();

        writeMessage(buffer);
    }
}

//insert the throttle note right after the log content, so it stays in front of the
//trailing parts of the pattern such as the code line
static void insertThrottleNote(QList<FormatPart>& messageParts, const QString& note,
                               ColorFormatter& noteStyle) {
    for (int i = 0; i < messageParts.size(); ++i) {
        if (messageParts[i].type == FormatPart::Type::Message) {
            messageParts.insert(i + 1, FormatPart(FormatPart::Type::Literal, note, false, &noteStyle));
            return;
        }
    }
    messageParts.append(FormatPart(FormatPart::Type::Literal, note, false, &noteStyle));
}

void MessageHandler::writeMessage(const QList<Message>& buffer) {
    for (const auto& buffered : buffer) {
        Message message = buffered;
        QString throttleNote;
        if (!throttle.accept(message, LogCollector::styleConfig.mThrottleConfig, &throttleNote)) {
            continue;
        }
        auto messagePart = formatter.format(message);
        if (!throttleNote.isEmpty()) {
            insertThrottleNote(messagePart, throttleNote, throttleNoteStyle);
        }
        if (message.noWrap) {
            messagePart = TextUtils::noWrapLayout(messagePart);
        }
        for (const auto& target : outputTargets) {
            target->writePart(messagePart, message);
        }
    }
}

void MessageHandler::flushAllTargets(bool force) {
    for (const auto& target : outputTargets) {
        if (force || target->needFlush()) {
            target->flush();
        }
    }
}

QString MessageHandler::sanitizeLogMessage(QString message) {
    static QRegularExpression nonAsciiPattern("[^\\x00-\\x7F]+");
    return message.replace(nonAsciiPattern, "\033[31m[NON-ASCII BLOCKED!]\033[0m");
}

bool MessageHandler::isTraceCollectionEnabled() const {
    return traceCollectionEnabled.loadAcquire() != 0;
}

QLOGCOLLECTOR_END_NAMESPACE
