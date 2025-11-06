#pragma once

#include <qobject.h>

#include "../outputtarget.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT SignalOutputTarget : public OutputTarget {
public:
    void writePart(const QList<FormatPart>& messageParts, const Message& message) override;
};

QLOGCOLLECTOR_END_NAMESPACE