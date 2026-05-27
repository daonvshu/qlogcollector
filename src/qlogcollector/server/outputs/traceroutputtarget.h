#pragma once

#include <qobject.h>
#include <qset.h>
#include <qstandardpaths.h>
#include <qdir.h>

#include "rollingfileoutputtargetbase.h"
#include "fileoutputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

struct QLOGCOLLECTOR_EXPORT TracerOutputConfig {
    QString saveDir;
    QString baseFileName;
    int contentLimitLines;
    int fileLimitSize;

    TracerOutputConfig()
        : saveDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/logs")
        , baseFileName("log")
        , contentLimitLines(1000)
        , fileLimitSize(10)
    {}
};

class QLOGCOLLECTOR_EXPORT TracerOutputConfigBuilder {
public:
    TracerOutputConfigBuilder& saveDir(const QString& dir) {
        config.saveDir = dir;
        return *this;
    }

    TracerOutputConfigBuilder& baseFileName(const QString& name) {
        config.baseFileName = name;
        return *this;
    }

    TracerOutputConfigBuilder& contentLimitLines(int lines) {
        config.contentLimitLines = lines;
        return *this;
    }

    TracerOutputConfigBuilder& fileLimitSize(int size) {
        config.fileLimitSize = size;
        return *this;
    }

    TracerOutputConfig build() const {
        QDir dir(config.saveDir);
        if (!dir.exists()) {
            if (!dir.mkpath(config.saveDir)) {
                qFatal("Can not create trace store directory.");
            }
        }
        return config;
    }

private:
    TracerOutputConfig config;
};

class QLOGCOLLECTOR_EXPORT TracerOutputTarget : public RollingFileOutputTargetBase {
public:
    explicit TracerOutputTarget(const TracerOutputConfigBuilder& configBuilder);

    void writePart(const QList<FormatPart>& messageParts, const Message& message) override;
    bool enableTraceCollection() const override;
    void followFileOutputConfig(const FileOutputConfig& fileConfig);

private:
    explicit TracerOutputTarget(const TracerOutputConfig& config);

    TracerOutputConfig config;
    QSet<QString> persistedTraceIds;

protected:
    QString fileSuffix() const override;
    QString filePrefix() const override;
    void onFileOpened(QTextStream& textStream) override;
};

QLOGCOLLECTOR_END_NAMESPACE
