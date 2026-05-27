#include "outputtarget.h"

#include "outputs/stdoutputtarget.h"
#include "outputs/win32consoleapptarget.h"
#include "outputs/win32debugconsoletarget.h"

#include "logcollector.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

QLOGCOLLECTOR_BEGIN_NAMESPACE

static bool checkCurrentDebugging() {
#ifdef Q_OS_WIN
#ifdef _M_X64
    void *PEB = (void*)(__readgsqword(0x60));
#elif _M_IX86
    void *PEB = (void*)(__readfsdword(0x30));
#endif
    return (bool)*(unsigned char*)((unsigned char*)PEB + 0x002);
#else
    return false;
#endif
}

static bool isConsoleApp() {
#ifdef Q_OS_WIN
    return GetConsoleWindow() != nullptr;
#else
    return false;
#endif
}

OutputTarget* OutputTarget::currentConsoleOutput(Ide ide, bool runWithPty) {
    OutputTarget* output = nullptr;
    switch (ide) {
        case Ide::unspecified:
            output = new StdOutputTarget;
            break;
        case Ide::clion: {
            if (isConsoleApp()) {
                if (runWithPty) {
                    if (checkCurrentDebugging()) {
                        output = new StdOutputTarget;
                    } else {
                        output = new Win32ConsoleAppTarget;
                        LogCollector::styleConfig.systemCodePage();
                    }
                } else {
                    output = new StdOutputTarget;
                }
            } else {
                output = new StdOutputTarget;
            }
        }
            break;
        case Ide::vs: {
            if (isConsoleApp()) {
                output = new Win32ConsoleAppTarget;
                LogCollector::styleConfig.systemCodePage();
                LogCollector::styleConfig.disableUnderlineStyle();
            } else {
                output = new Win32DebugConsoleTarget;
            }
        }
            break;
        case Ide::vscode:
            output = new StdOutputTarget;
            LogCollector::styleConfig.systemCodePage();
            break;
        case Ide::qtcreator: {
            if (checkCurrentDebugging()) {
#ifdef Q_CC_MSVC
                output = new Win32DebugConsoleTarget(true);
#else
                output = new StdOutputTarget;
#endif
            } else {
                output = new StdOutputTarget;
            }
            LogCollector::styleConfig.systemCodePage();
            LogCollector::styleConfig.disableLighterStyle();
        }
            break;
    }
    return output;
}

bool OutputTarget::needFlush() {
    return true;
}

void OutputTarget::flush() {
}

bool OutputTarget::enableTraceCollection() const {
    return false;
}

bool OutputTarget::isSystemCodePage() {
    return LogCollector::styleConfig.useSystemCodePage;
}

bool OutputTarget::isLighterColorEnabled() {
    return LogCollector::styleConfig.mLighterColorEnabled;
}

bool OutputTarget::isUnderlineEnabled() {
    return LogCollector::styleConfig.mUnderlineEnabled;
}

QLOGCOLLECTOR_END_NAMESPACE
