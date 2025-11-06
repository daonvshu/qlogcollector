#pragma once

#include <qmutex.h>
#include <qobject.h>
#include <qthread.h>
#include <qsharedpointer.h>
#include <qwaitcondition.h>
#include <qatomic.h>

#include <qlogcollector/comm/global.h>
#include <qlogcollector/comm/message.h>

#include "outputtarget.h"
#include "patternformatter.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

class MessageHandler : public QThread {
public:
    explicit MessageHandler(QObject* parent = nullptr);

    void addOutputTarget(OutputTarget* outputTarget);

    void setMessageFormat(const QString& format);

    void processMessage(const Message& message);

    void flush();

    void exit();

    void run() override;

private:
    QList<QSharedPointer<OutputTarget>> outputTargets;
    PatternFormatter formatter;
    QMutex messageMutex;
    QWaitCondition messageCondition;
    QList<Message> messages;
    QAtomicInt forceFlush = 0;

private:
    void writeMessage(const QList<Message>& buffer);
    void flushAllTargets(bool force);
};

QLOGCOLLECTOR_END_NAMESPACE