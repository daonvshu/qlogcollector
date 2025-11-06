#pragma once

#include <qobject.h>

#include <qlogcollector/comm/global.h>
#include <qlogcollector/comm/message.h>

#include "patternformatter.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

enum class Ide {
    unspecified,
    clion,
    vs,
    vscode,
    qtcreator,
};

class QLOGCOLLECTOR_EXPORT OutputTarget {
public:
    static OutputTarget* currentConsoleOutput(Ide ide = Ide::unspecified, bool runWithPty = true);

    virtual ~OutputTarget() = default;

    virtual void writePart(const QList<FormatPart>& messageParts, const Message& message) = 0;

    virtual bool needFlush();

    virtual void flush();

protected:
    static bool isSystemCodePage();
    static bool isLighterColorEnabled();
    static bool isUnderlineEnabled();
};

QLOGCOLLECTOR_END_NAMESPACE