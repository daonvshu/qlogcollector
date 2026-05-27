#pragma once

#include "outputstyleconfig.h"
#include "outputtarget.h"
#include "outputs/memoryoutputtarget.h"

#include <qobject.h>
#include <qlogcollector/comm/global.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT LogCollector : public QObject {
    Q_DISABLE_COPY(LogCollector)

public:
    class QLOGCOLLECTOR_EXPORT Bootstrap {
    public:
        explicit Bootstrap();

        Bootstrap& style(const OutputStyleConfig& config);
        Bootstrap& style(const QString& projectRootPath, int wordWrap,
                         bool simpleCodeLine = false,
                         bool systemCodePage = false,
                         bool nonAsciiCheck = false);
        Bootstrap& messageFormat(const QString& format);
        Bootstrap& console(Ide ide = Ide::unspecified, bool runWithPty = true);
        Bootstrap& output(OutputTarget* outputTarget);
        Bootstrap& fileOutput(const QString& saveDir,
                              const QString& baseFileName = "log",
                              int contentLimitLines = 1000,
                              int fileLimitSize = 10,
                              bool machineEncodeMode = false);
        Bootstrap& tracerOutput(const QString& saveDir = QString(),
                                const QString& baseFileName = "log",
                                int contentLimitLines = 1000,
                                int fileLimitSize = 10);
        Bootstrap& memoryOutput(int limitSize = 3000, bool styledText = false);
        Bootstrap& bindFatalSignal(bool enable = true);
        Bootstrap& registerQtMessageHandler(bool enable = true);
        void start();

    private:
        OutputStyleConfig styleConfigData;
        QString msgFormat;
        QList<OutputTarget*> outputs;
        Ide ide = Ide::unspecified;
        bool useConsole = false;
        bool runWithPty = true;
        bool bindFatal = false;
        bool registerQtHandler = true;
    };

    static Bootstrap quickStart();
    static MemoryOutputTarget* getMemoryOutput();

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
