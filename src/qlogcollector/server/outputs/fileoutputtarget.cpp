#include "fileoutputtarget.h"

#include <qdir.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

FileOutputTarget::FileOutputTarget(const FileOutputConfigBuilder& configBuilder)
    : config(configBuilder.build())
{
    initFileIndex();
}

FileOutputTarget::~FileOutputTarget() {
    closeCurFile();
}

void FileOutputTarget::writePart(const QList<FormatPart>& messageParts, const Message& message) {
    if (curFile == nullptr) {
        openNextFile();
    }

    if (config.machineEncodeMode) {
        *curStream << message.dumpToJson() << ",";
    } else {
        for (const auto &part : messageParts) {
            auto content = part.content;
            if (part.type == FormatPart::Type::Message) {
                content = TextUtils::removeColors(content);
            }
            *curStream << content;
            if (part.lineBreak) {
                *curStream << "\n";
                currentFileWriteLines++;
            }
        }
        *curStream << "\n";
    }
    currentFileWriteLines++;

    if (currentFileWriteLines >= config.contentLimitLines) {
        closeCurFile();
        openNextFile();
    }

    clearOldFile();
}

void FileOutputTarget::flush() {
    if (curStream) {
        curStream->flush();
    }
}

void FileOutputTarget::initFileIndex() {
    QDir dir(config.saveDir);
    auto files = dir.entryList(QStringList() << QString("%1_*.log").arg(config.baseFileName), QDir::Files, QDir::Name);

    int maxIndex = -1;
    static QRegularExpression re(
        QString("^%1_\\d{4}-\\d{2}-\\d{2}_(\\d+)\\.log$")
        .arg(QRegularExpression::escape(config.baseFileName))
    );
    for (const auto &file : files) {
        auto match = re.match(file);
        if (match.hasMatch()) {
            bool ok;
            int idx = match.captured(1).toInt(&ok);
            if (ok && idx > maxIndex) {
                maxIndex = idx;
            }
        }
    }
    fileIndex = maxIndex + 1;
}

QString FileOutputTarget::makeFileName() const {
    QString dateStr = QDate::currentDate().toString("yyyy-MM-dd");
    return QString("%1/%2_%3_%4.log")
            .arg(config.saveDir)
            .arg(config.baseFileName)
            .arg(dateStr)
            .arg(fileIndex);
}

void FileOutputTarget::openNextFile() {
    auto fileName = makeFileName();
    curFile = new QFile(fileName);
    if (!curFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qFatal("Failed to create log file: %s", qPrintable(fileName));
    }
    curStream = new QTextStream(curFile);
    curStream->setCodec("UTF-8");
    currentFileWriteLines = 0;
}

void FileOutputTarget::closeCurFile() {
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

void FileOutputTarget::clearOldFile() const {
    if (config.fileLimitSize <= 0) {
        return;
    }
    QDir dir(config.saveDir);
    auto files = dir.entryList(QStringList() << QString("%1_*.log").arg(config.baseFileName),QDir::Files, QDir::Name);
    if (files.size() <= config.fileLimitSize) {
        return;
    }

    struct LogFileInfo {
        QString fileName;
        int index;
    };
    QList<LogFileInfo> logFiles;

    static QRegularExpression re(
        QString("^%1_\\d{4}-\\d{2}-\\d{2}_(\\d+)\\.log$")
        .arg(QRegularExpression::escape(config.baseFileName))
    );

    for (const auto &file : files) {
        auto match = re.match(file);
        if (match.hasMatch()) {
            bool ok;
            int idx = match.captured(1).toInt(&ok);
            if (ok) {
                logFiles.append({ file, idx });
            }
        }
    }

    std::sort(logFiles.begin(), logFiles.end(), [](const LogFileInfo &a, const LogFileInfo &b) {
        return a.index < b.index;
    });

    while (logFiles.size() > config.fileLimitSize) {
        auto oldestFile = logFiles.front().fileName;
        dir.remove(oldestFile);
        logFiles.pop_front();
    }
}

QLOGCOLLECTOR_END_NAMESPACE
