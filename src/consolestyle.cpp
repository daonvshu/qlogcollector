#include "../include/consolestyle.h"

#include <qglobal.h>

#ifdef Q_OS_WIN
#include <intrin.h>
#endif

namespace logcollector {

    ConsoleOutputStyleConfig::ConsoleOutputStyleConfig()
            : mOutputTarget(ConsoleOutputTarget::TARGET_STANDARD_OUTPUT)
            , mSimpleCodeLine(false)
            , mLogLineWidth(0)
            , win32App(true)
            , useSystemCodePage(false)
            , mLighterColorEnabled(true)
            , mUnderlineEnabled(true)
            , win32DebugConsoleWithStdColorStyle(false)
    {}

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::target(const ConsoleOutputTarget &outputTarget) {
        mOutputTarget = outputTarget;
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::simpleCodeLine() {
        mSimpleCodeLine = true;
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::disableLighterStyle() {
        mLighterColorEnabled = false;
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::disableUnderlineStyle() {
        mUnderlineEnabled = false;
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::wordWrap(int lineWidth) {
        Q_ASSERT(lineWidth > 0);
        if (mLogLineWidth == 0) {
            mLogLineWidth = qMax(60, lineWidth);
        } else {
            mLogLineWidth = qMin(qMax(60, lineWidth), mLogLineWidth);
        }
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::systemCodePage() {
        useSystemCodePage = true;
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::consoleApp() {
        win32App = false;
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::windowApp() {
        win32App = true;
        return *this;
    }

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

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::ide_clion(bool runWithPty) {
        if (win32App) {
            if (runWithPty) {
                if (checkCurrentDebugging()) {
                    mOutputTarget = ConsoleOutputTarget::TARGET_STANDARD_OUTPUT;
                }
                //cannot print in run debug mode, any print invisible, consider disable 'run with pty'
            } else {
                mOutputTarget = ConsoleOutputTarget::TARGET_STANDARD_OUTPUT;
            }
        } else {
            if (runWithPty) {
                if (checkCurrentDebugging()) {
                    mOutputTarget = ConsoleOutputTarget::TARGET_STANDARD_OUTPUT;
                } else {
                    mOutputTarget = ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP;
                    systemCodePage();
                }
            } else {
                mOutputTarget = ConsoleOutputTarget::TARGET_STANDARD_OUTPUT;
            }
        }
        if (runWithPty) {
            //run with pty mode limit column width 120 characters, this is the existing problems
            if (!checkCurrentDebugging()) {
                if (mLogLineWidth == 0) {
                    mLogLineWidth = 72;
                } else {
                    mLogLineWidth = qMin(mLogLineWidth, 72);
                }
            }
        }
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::ide_vs() {
        if (win32App) {
            //not support to set color style in console window
            mOutputTarget = ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE;
        } else {
            mOutputTarget = ConsoleOutputTarget::TARGET_WIN32_CONSOLE_APP;
            systemCodePage();
            disableUnderlineStyle();
        }
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::ide_vscode() {
        if (win32App) {
            if (checkCurrentDebugging()) {
                //cannot print in run debug mode, any print invisible
            }
        }
        //supports standard output for any mode
        mOutputTarget = ConsoleOutputTarget::TARGET_STANDARD_OUTPUT;
        systemCodePage();
        return *this;
    }

    ConsoleOutputStyleConfig &ConsoleOutputStyleConfig::ide_qtcreator() {
        if (checkCurrentDebugging()) {
#ifdef Q_CC_MSVC
            //can be print only in this mode
            mOutputTarget = ConsoleOutputTarget::TARGET_WIN32_DEBUG_CONSOLE;
            win32DebugConsoleWithStdColorStyle = true;
#else
            mOutputTarget = ConsoleOutputTarget::TARGET_STANDARD_OUTPUT;
#endif
        } else {
            mOutputTarget = ConsoleOutputTarget::TARGET_STANDARD_OUTPUT;
        }
        systemCodePage();
        disableLighterStyle();
        return *this;
    }
}