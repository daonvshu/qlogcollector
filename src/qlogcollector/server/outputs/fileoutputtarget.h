#pragma once

#include <qobject.h>
#include <qstandardpaths.h>
#include <qfile.h>
#include <qtextstream.h>

#include "../outputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

struct QLOGCOLLECTOR_EXPORT FileOutputConfig {
    QString saveDir;
    QString baseFileName;
    int contentLimitLines;
    int fileLimitSize;
    bool machineEncodeMode;

    FileOutputConfig()
        : saveDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/logs")
        , baseFileName("log")
        , contentLimitLines(1000)
        , fileLimitSize(10)
        , machineEncodeMode(false)
    {}
};

class QLOGCOLLECTOR_EXPORT FileOutputConfigBuilder {
public:
    FileOutputConfigBuilder& saveDir(const QString &dir) {
        config.saveDir = dir;
        return *this;
    }

    FileOutputConfigBuilder& baseFileName(const QString &name) {
        config.baseFileName = name;
        return *this;
    }

    FileOutputConfigBuilder& contentLimitLines(int lines) {
        config.contentLimitLines = lines;
        return *this;
    }

    FileOutputConfigBuilder& fileLimitSize(int size) {
        config.fileLimitSize = size;
        return *this;
    }

    FileOutputConfigBuilder& machineEncodeMode(bool mode) {
        config.machineEncodeMode = mode;
        return *this;
    }

    FileOutputConfig build() const {
        return config;
    }

private:
    FileOutputConfig config;
};

class QLOGCOLLECTOR_EXPORT FileOutputTarget : public OutputTarget {
public:
    explicit FileOutputTarget(const FileOutputConfigBuilder& configBuilder);
    ~FileOutputTarget() override;

    void writePart(const QList<FormatPart>& messageParts, const Message& message) override;

    void flush() override;

private:
    FileOutputConfig config;
    int currentFileWriteLines = 0;
    int fileIndex = 0;
    QFile *curFile = nullptr;
    QTextStream *curStream = nullptr;

private:
    void initFileIndex();
    QString makeFileName() const;
    void openNextFile();
    void closeCurFile();
    void clearOldFile() const;
};

QLOGCOLLECTOR_END_NAMESPACE