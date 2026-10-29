#include "printmarker.h"

#include "colors/styledstring.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

QChar allowNonAsciiPrintMarker() {
    return QChar(0x2060);
}

QChar noWrapPrintMarker() {
    return QChar(0x2062);
}

QChar logThrottleMarker() {
    return QChar(0x2063);
}

QLOGCOLLECTOR_END_NAMESPACE

QLogCollector::StyledString allowNonAsciiPrint() {
    return QLogCollector::StyledString(QLogCollector::allowNonAsciiPrintMarker(), false, false);
}

QLogCollector::StyledString noWrapPrint() {
    return QLogCollector::StyledString(QLogCollector::noWrapPrintMarker(), false, false);
}

QLogCollector::StyledString throttlePrint() {
    return QLogCollector::StyledString(QLogCollector::logThrottleMarker(), false, false);
}
