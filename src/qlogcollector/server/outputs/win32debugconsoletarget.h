#pragma once

#include <qobject.h>

#include "../outputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT Win32DebugConsoleTarget : public OutputTarget {
public:
    explicit Win32DebugConsoleTarget(bool useStdColorStyle = false);

    void writePart(const QList<FormatPart>& messageParts, const Message& message) override;

private:
    bool useStdColorStyle;
};

QLOGCOLLECTOR_END_NAMESPACE