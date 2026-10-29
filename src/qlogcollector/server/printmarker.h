#pragma once

#include <qchar.h>

#include <qlogcollector/comm/global.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

//StyledString is defined in colors/styledstring.h, include that header to use the
//print helpers declared below
class StyledString;

//Invisible markers written by the print helpers in front of a log statement.
//They are stripped by the message handler before the log is formatted, so the
//helpers can be combined, e.g. noWrapPrint() with throttlePrint().
QLOGCOLLECTOR_EXPORT QChar allowNonAsciiPrintMarker();
QLOGCOLLECTOR_EXPORT QChar noWrapPrintMarker();

//Marker of the call site throttle: writing it at the beginning of a log statement
//enables throttling for that call site.
QLOGCOLLECTOR_EXPORT QChar logThrottleMarker();

//Length of the throttle marker: the marker itself plus the separator space inserted by QDebug
constexpr int logThrottleMarkerLength = 2;

QLOGCOLLECTOR_END_NAMESPACE

//Print helpers, the marker they return must be the first argument of a log statement
extern QLogCollector::StyledString allowNonAsciiPrint();
extern QLogCollector::StyledString noWrapPrint();
extern QLogCollector::StyledString throttlePrint();
