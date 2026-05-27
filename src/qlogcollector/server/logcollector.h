#pragma once

#include "outputstyleconfig.h"
#include "outputtarget.h"

#include <qobject.h>
#include <qlogcollector/comm/global.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT LogCollector : public QObject {
    Q_DISABLE_COPY(LogCollector)

public:
    static void init();

    static void registerLog();

    static void addOutputTarget(OutputTarget* outputTarget);

    static void setMessageFormat(const QString& format);

    static void collectorMessageHandle(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    static void flushLogs();

    static void bindSignalFatal();

    static QString exportTraceContext();

    static void importTraceContext(const QString& context);

    static void clearTraceContext();

private:
    friend void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

public:
    static OutputStyleConfig styleConfig;
};

QLOGCOLLECTOR_END_NAMESPACE
