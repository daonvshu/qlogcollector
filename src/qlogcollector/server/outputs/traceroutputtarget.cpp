#include "traceroutputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

static RollingFileOutputTargetBase::RollingConfig makeRollingConfig(const TracerOutputConfig& config) {
    RollingFileOutputTargetBase::RollingConfig rolling;
    rolling.saveDir = config.saveDir;
    rolling.baseFileName = config.baseFileName;
    rolling.contentLimitLines = config.contentLimitLines;
    rolling.fileLimitSize = config.fileLimitSize;
    return rolling;
}

TracerOutputTarget::TracerOutputTarget(const TracerOutputConfigBuilder& configBuilder)
    : TracerOutputTarget(configBuilder.build())
{}

TracerOutputTarget::TracerOutputTarget(const TracerOutputConfig& config)
    : RollingFileOutputTargetBase(makeRollingConfig(config))
    , config(config)
{}

void TracerOutputTarget::writePart(const QList<FormatPart>&, const Message& message) {
    if (message.traceId.isEmpty() || message.traceContextBase64.isEmpty()) {
        return;
    }
    if (persistedTraceIds.contains(message.traceId)) {
        return;
    }

    auto safeTraceId = message.traceId;
    safeTraceId.replace("\"", "\"\"");
    auto safeContext = message.traceContextBase64;
    safeContext.replace("\"", "\"\"");

    *stream() << "\"" << safeTraceId << "\",\"" << safeContext << "\"\n";
    persistedTraceIds.insert(message.traceId);
    markLineWritten(1);
}

bool TracerOutputTarget::enableTraceCollection() const {
    return true;
}

void TracerOutputTarget::followFileOutputConfig(const FileOutputConfig& fileConfig) {
    config.saveDir = fileConfig.saveDir;
    config.baseFileName = fileConfig.baseFileName;
    config.contentLimitLines = fileConfig.contentLimitLines;
    config.fileLimitSize = fileConfig.fileLimitSize;
    persistedTraceIds.clear();

    RollingFileOutputTargetBase::RollingConfig rolling;
    rolling.saveDir = config.saveDir;
    rolling.baseFileName = config.baseFileName;
    rolling.contentLimitLines = config.contentLimitLines;
    rolling.fileLimitSize = config.fileLimitSize;
    applyRollingConfig(rolling);
}

QString TracerOutputTarget::fileSuffix() const {
    return "csv";
}

QString TracerOutputTarget::filePrefix() const {
    return "_trace_";
}

void TracerOutputTarget::onFileOpened(QTextStream& textStream) {
    textStream << "trace_id,trace_context_base64\n";
}

QLOGCOLLECTOR_END_NAMESPACE
