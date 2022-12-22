#pragma once

namespace logcollector {

    enum class ConsoleOutputTarget {
        TARGET_STANDARD_OUTPUT,
        TARGET_WIN32_CONSOLE_APP,
        TARGET_WIN32_DEBUG_CONSOLE,
    };

    class ConsoleOutputStyleConfig {
    public:
        explicit ConsoleOutputStyleConfig();

        ConsoleOutputStyleConfig& target(const ConsoleOutputTarget& outputTarget);

        ConsoleOutputStyleConfig& simpleCodeLine();

        ConsoleOutputStyleConfig& wordWrap(int lineWidth);

        ConsoleOutputStyleConfig& consoleApp();

        ConsoleOutputStyleConfig& windowApp();

        ConsoleOutputStyleConfig& ide_clion(bool runWithPty = true);

        ConsoleOutputStyleConfig& ide_vs();

        ConsoleOutputStyleConfig& ide_vscode();

        ConsoleOutputStyleConfig& ide_qtcreator();

    private:
        ConsoleOutputTarget mOutputTarget;
        bool mSimpleCodeLine;
        int mLogLineWidth;
        bool win32App;

        friend class Console;
        friend class StyledString;
        friend class QLogCollector;
    };

    extern ConsoleOutputStyleConfig styleConfig;
}