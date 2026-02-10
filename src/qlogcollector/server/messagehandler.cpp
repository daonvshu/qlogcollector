#include "messagehandler.h"

#include <logcollector.h>

#include "outputstyleconfig.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

MessageHandler::MessageHandler(QObject* parent)
    : QThread(parent)
{
    setMessageFormat("[%d{HH:mm:ss.zzz}[fg=g]] [%t] %c %p-> %m (%f)");
}

void MessageHandler::addOutputTarget(OutputTarget* outputTarget) {
    outputTargets << QSharedPointer<OutputTarget>(outputTarget);
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

void MessageHandler::writeMessage(const QList<Message>& buffer) {
    for (const auto& message : buffer) {
        auto messagePart = formatter.format(message);
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

QLOGCOLLECTOR_END_NAMESPACE
