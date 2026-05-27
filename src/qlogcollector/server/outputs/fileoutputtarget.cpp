#include "fileoutputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

static RollingFileOutputTargetBase::RollingConfig makeRollingConfig(const FileOutputConfig& config) {
    RollingFileOutputTargetBase::RollingConfig rolling;
    rolling.saveDir = config.saveDir;
    rolling.baseFileName = config.baseFileName;
    rolling.contentLimitLines = config.contentLimitLines;
    rolling.fileLimitSize = config.fileLimitSize;
    return rolling;
}

FileOutputTarget::FileOutputTarget(const FileOutputConfigBuilder& configBuilder)
    : FileOutputTarget(configBuilder.build())
{}

FileOutputTarget::FileOutputTarget(const FileOutputConfig& config)
    : RollingFileOutputTargetBase(makeRollingConfig(config))
    , config(config)
{}

void FileOutputTarget::writePart(const QList<FormatPart>& messageParts, const Message& message) {
    auto* out = stream();
    int lineCount = 0;

    if (config.machineEncodeMode) {
        *out << message.dumpToJson().toUtf8().toBase64() << ",";
    } else {
        for (const auto& part : messageParts) {
            auto content = part.content;
            if (part.type == FormatPart::Type::Message) {
                content = TextUtils::removeColors(content);
            }
            *out << content;
            if (part.lineBreak) {
                *out << "\n";
                lineCount++;
            }
        }
        *out << "\n";
    }
    lineCount++;
    markLineWritten(lineCount);
}

const FileOutputConfig& FileOutputTarget::outputConfig() const {
    return config;
}

QString FileOutputTarget::fileSuffix() const {
    return "log";
}

QString FileOutputTarget::filePrefix() const {
    return "_";
}

void FileOutputTarget::onFileOpened(QTextStream&) {
}

QLOGCOLLECTOR_END_NAMESPACE
