#include "win32debugconsoletarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

Win32DebugConsoleTarget::Win32DebugConsoleTarget(bool useStdColorStyle)
    : useStdColorStyle(useStdColorStyle)
{
}

void Win32DebugConsoleTarget::writePart(const QList<FormatPart>& messageParts, const Message& message) {
}

QLOGCOLLECTOR_END_NAMESPACE
