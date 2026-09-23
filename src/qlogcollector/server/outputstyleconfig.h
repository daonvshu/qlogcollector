#pragma once

#include <qstring.h>

#include <qlogcollector/comm/global.h>

#include "logthrottle.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT OutputStyleConfig {
public:
    explicit OutputStyleConfig();

    OutputStyleConfig& simpleCodeLine();

    OutputStyleConfig& wordWrap(int lineWidth);

    OutputStyleConfig& systemCodePage();

    OutputStyleConfig& disableLighterStyle();

    OutputStyleConfig& disableUnderlineStyle();

    OutputStyleConfig& disableNonAscii();

    OutputStyleConfig& print3rdCodeLine(bool enable = true);

    OutputStyleConfig& projectSourceCodeRootPath(const QString& path);

    OutputStyleConfig& logThrottle(bool enable = true);

    OutputStyleConfig& logThrottle(int initialBurst, int baseIntervalMs,
                                   int maxIntervalMs, int idleResetMs);

private:
    bool mSimpleCodeLine;
    int mLogLineWidth;
    QString codeRootPath;

    bool useSystemCodePage;
    bool mLighterColorEnabled;
    bool mUnderlineEnabled;
    bool mNonAsciiCheckEnabled;
    bool mPrint3rdCodeLine;
    LogThrottleConfig mThrottleConfig;

    friend class LogCollector;
    friend class OutputTarget;
    friend class ColorFormatter;
    friend class TextUtils;
    friend class MessageHandler;
};

QLOGCOLLECTOR_END_NAMESPACE
