#include "rollingfileoutputtargetbase.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

RollingFileOutputTargetBase::RollingFileOutputTargetBase(const RollingConfig& rollingConfig)
    : rollingConfig(rollingConfig)
{
    QDir dir(this->rollingConfig.saveDir);
    if (!dir.exists()) {
        if (!dir.mkpath(this->rollingConfig.saveDir)) {
            qFatal("Can not create output store directory.");
        }
    }
}

RollingFileOutputTargetBase::~RollingFileOutputTargetBase() {
    closeCurFile();
}

void RollingFileOutputTargetBase::flush() {
    if (curStream) {
        curStream->flush();
    }
}

QTextStream* RollingFileOutputTargetBase::stream() {
    ensureIndexInitialized();
    if (curStream == nullptr) {
        openNextFile();
    }
    return curStream;
}

void RollingFileOutputTargetBase::markLineWritten(int lineCount) {
    ensureIndexInitialized();
    currentFileWriteLines += lineCount;
    if (currentFileWriteLines >= rollingConfig.contentLimitLines) {
        closeCurFile();
        openNextFile();
    }
    clearOldFile();
}

void RollingFileOutputTargetBase::applyRollingConfig(const RollingConfig& rollingConfig) {
    this->rollingConfig = rollingConfig;
    QDir dir(this->rollingConfig.saveDir);
    if (!dir.exists()) {
        if (!dir.mkpath(this->rollingConfig.saveDir)) {
            qFatal("Can not create output store directory.");
        }
    }
    closeCurFile();
    indexInitialized = false;
}

void RollingFileOutputTargetBase::ensureIndexInitialized() {
    if (indexInitialized) {
        return;
    }
    initFileIndex();
    indexInitialized = true;
}

void RollingFileOutputTargetBase::initFileIndex() {
    QDir dir(rollingConfig.saveDir);
    auto files = dir.entryList(QStringList() << QString("%1%2*.%3")
        .arg(rollingConfig.baseFileName)
        .arg(filePrefix())
        .arg(fileSuffix()), QDir::Files, QDir::Name);

    int maxIndex = -1;
    QRegularExpression re(
        QString("^%1%2\\d{4}-\\d{2}-\\d{2}_(\\d+)\\.%3$")
            .arg(QRegularExpression::escape(rollingConfig.baseFileName))
            .arg(QRegularExpression::escape(filePrefix()))
            .arg(QRegularExpression::escape(fileSuffix()))
    );
    for (const auto& file : files) {
        auto match = re.match(file);
        if (match.hasMatch()) {
            bool ok = false;
            int idx = match.captured(1).toInt(&ok);
            if (ok && idx > maxIndex) {
                maxIndex = idx;
            }
        }
    }
    fileIndex = maxIndex + 1;
    currentFileWriteLines = 0;
}

QString RollingFileOutputTargetBase::makeFileName() const {
    QString dateStr = QDate::currentDate().toString("yyyy-MM-dd");
    return QString("%1/%2%3%4_%5.%6")
        .arg(rollingConfig.saveDir)
        .arg(rollingConfig.baseFileName)
        .arg(filePrefix())
        .arg(dateStr)
        .arg(fileIndex)
        .arg(fileSuffix());
}

void RollingFileOutputTargetBase::openNextFile() {
    auto fileName = makeFileName();
    curFile = new QFile(fileName);
    if (!curFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qFatal("Failed to create output file: %s", qPrintable(fileName));
    }
    curStream = new QTextStream(curFile);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    curStream->setEncoding(QStringConverter::Utf8);
#else
    curStream->setCodec("UTF-8");
#endif
    currentFileWriteLines = 0;
    fileIndex++;
    onFileOpened(*curStream);
}

void RollingFileOutputTargetBase::closeCurFile() {
    if (curStream) {
        curStream->flush();
        delete curStream;
        curStream = nullptr;
    }
    if (curFile) {
        curFile->close();
        delete curFile;
        curFile = nullptr;
    }
}

void RollingFileOutputTargetBase::clearOldFile() const {
    if (rollingConfig.fileLimitSize <= 0) {
        return;
    }

    QDir dir(rollingConfig.saveDir);
    auto files = dir.entryList(QStringList() << QString("%1%2*.%3")
        .arg(rollingConfig.baseFileName)
        .arg(filePrefix())
        .arg(fileSuffix()), QDir::Files, QDir::Name);
    if (files.size() <= rollingConfig.fileLimitSize) {
        return;
    }

    struct FileInfo {
        QString fileName;
        int index;
    };
    QList<FileInfo> fileInfos;

    QRegularExpression re(
        QString("^%1%2\\d{4}-\\d{2}-\\d{2}_(\\d+)\\.%3$")
            .arg(QRegularExpression::escape(rollingConfig.baseFileName))
            .arg(QRegularExpression::escape(filePrefix()))
            .arg(QRegularExpression::escape(fileSuffix()))
    );

    for (const auto& file : files) {
        auto match = re.match(file);
        if (match.hasMatch()) {
            bool ok = false;
            int idx = match.captured(1).toInt(&ok);
            if (ok) {
                fileInfos.append({ file, idx });
            }
        }
    }

    std::sort(fileInfos.begin(), fileInfos.end(), [](const FileInfo& a, const FileInfo& b) {
        return a.index < b.index;
    });

    while (fileInfos.size() > rollingConfig.fileLimitSize) {
        auto oldestFile = fileInfos.front().fileName;
        dir.remove(oldestFile);
        fileInfos.pop_front();
    }
}

QLOGCOLLECTOR_END_NAMESPACE
