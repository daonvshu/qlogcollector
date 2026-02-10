#include "outputstyleconfig.h"

#include <qglobal.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

OutputStyleConfig::OutputStyleConfig()
    : mSimpleCodeLine(false)
    , mLogLineWidth(0)
    , useSystemCodePage(false)
    , mLighterColorEnabled(true)
    , mUnderlineEnabled(true)
    , mNonAsciiCheckEnabled(false)
{}

OutputStyleConfig &OutputStyleConfig::simpleCodeLine() {
    mSimpleCodeLine = true;
    return *this;
}

OutputStyleConfig &OutputStyleConfig::disableLighterStyle() {
    mLighterColorEnabled = false;
    return *this;
}

OutputStyleConfig &OutputStyleConfig::disableUnderlineStyle() {
    mUnderlineEnabled = false;
    return *this;
}

OutputStyleConfig& OutputStyleConfig::disableNonAscii() {
    mNonAsciiCheckEnabled = true;
    return *this;
}

OutputStyleConfig &OutputStyleConfig::wordWrap(int lineWidth) {
    Q_ASSERT(lineWidth > 0);
    if (mLogLineWidth == 0) {
        mLogLineWidth = qMax(60, lineWidth);
    } else {
        mLogLineWidth = qMin(qMax(60, lineWidth), mLogLineWidth);
    }
    return *this;
}

OutputStyleConfig &OutputStyleConfig::systemCodePage() {
    useSystemCodePage = true;
    return *this;
}

OutputStyleConfig &OutputStyleConfig::projectSourceCodeRootPath(const QString &path) {
    codeRootPath = path;
    return *this;
}

QLOGCOLLECTOR_END_NAMESPACE