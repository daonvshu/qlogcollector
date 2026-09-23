#include "messagehandler.h"

#include <logcollector.h>
#include <qregularexpression.h>

#include "outputstyleconfig.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

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
    bool allowNonAscii = message.log.startsWith(QChar(0x2060));
    if (LogCollector::styleConfig.mNonAsciiCheckEnabled && !allowNonAscii) {
        auto filterMsg = message;
        filterMsg.log = sanitizeLogMessage(filterMsg.log);
        messages.append(filterMsg);
    } else {
        if (allowNonAscii) {
            auto filterMsg = message;
            filterMsg.log = message.log.mid(2);
            messages.append(filterMsg);
        } else {
            messages.append(message);
        }
    }
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
    //the throttle marker is library internal information, keep it out of the non-ascii check
    QString marker;
    if (message.startsWith(logThrottleMarker())) {
        marker = message.left(logThrottleMarkerLength);
        message.remove(0, logThrottleMarkerLength);
    }
    static QRegularExpression nonAsciiPattern("[^\\x00-\\x7F]+");
    return marker + message.replace(nonAsciiPattern, "\033[31m[NON-ASCII BLOCKED!]\033[0m");
}

bool MessageHandler::isTraceCollectionEnabled() const {
    return traceCollectionEnabled.loadAcquire() != 0;
}

QLOGCOLLECTOR_END_NAMESPACE
