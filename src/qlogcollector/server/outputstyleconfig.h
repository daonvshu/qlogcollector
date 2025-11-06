#pragma once

#include <qstring.h>

#include <qlogcollector/comm/global.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT OutputStyleConfig {
public:
    explicit OutputStyleConfig();

    OutputStyleConfig& simpleCodeLine();

    OutputStyleConfig& wordWrap(int lineWidth);

    OutputStyleConfig& systemCodePage();

    OutputStyleConfig& disableLighterStyle();

    OutputStyleConfig& disableUnderlineStyle();

    OutputStyleConfig& projectSourceCodeRootPath(const QString& path);

private:
    bool mSimpleCodeLine;
    int mLogLineWidth;
    QString codeRootPath;

    bool useSystemCodePage;
    bool mLighterColorEnabled;
    bool mUnderlineEnabled;

    friend class LogCollector;
    friend class OutputTarget;
    friend class ColorFormatter;
    friend class TextUtils;
};

QLOGCOLLECTOR_END_NAMESPACE