#pragma once

#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qregularexpression.h>

#include "../outputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT RollingFileOutputTargetBase : public OutputTarget {
public:
    struct RollingConfig {
        QString saveDir;
        QString baseFileName;
        int contentLimitLines = 1000;
        int fileLimitSize = 10;
    };

    explicit RollingFileOutputTargetBase(const RollingConfig& rollingConfig);
    ~RollingFileOutputTargetBase() override;

    void flush() override;

protected:
    QTextStream* stream();
    void markLineWritten(int lineCount = 1);
    void applyRollingConfig(const RollingConfig& rollingConfig);

    virtual QString fileSuffix() const = 0;
    virtual QString filePrefix() const = 0;
    virtual void onFileOpened(QTextStream& textStream) = 0;

private:
    RollingConfig rollingConfig;
    int currentFileWriteLines = 0;
    int fileIndex = 0;
    bool indexInitialized = false;
    QFile* curFile = nullptr;
    QTextStream* curStream = nullptr;

private:
    void ensureIndexInitialized();
    void initFileIndex();
    QString makeFileName() const;
    void openNextFile();
    void closeCurFile();
    void clearOldFile() const;
};

QLOGCOLLECTOR_END_NAMESPACE
