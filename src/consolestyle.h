#pragma once

namespace logcollector {

    enum class ConsoleOutputTarget {
        TARGET_STANDARD_OUTPUT,
        TARGET_WIN32_CONSOLE_APP,
        TARGET_WIN32_DEBUG_CONSOLE,
    };

    class ConsoleOutputStyleConfig {
    public:
        explicit ConsoleOutputStyleConfig()
            : mOutputTarget(ConsoleOutputTarget::TARGET_STANDARD_OUTPUT)
            , mSimpleCodeLine(false)
            , mLogLineWidth(0)
        {}

        ConsoleOutputStyleConfig& target(const ConsoleOutputTarget& outputTarget) {
            mOutputTarget = outputTarget;
            return *this;
        }

        ConsoleOutputStyleConfig& simpleCodeLine() {
            mSimpleCodeLine = true;
            return *this;
        }

        ConsoleOutputStyleConfig& lineWidth(int width) {
            mLogLineWidth = qMax(60, width);
            return *this;
        }

    private:
        ConsoleOutputTarget mOutputTarget;
        bool mSimpleCodeLine;
        int mLogLineWidth;

        friend class Console;
        friend class StyledString;
        friend class QLogCollector;
    };

    extern ConsoleOutputStyleConfig styleConfig;
}