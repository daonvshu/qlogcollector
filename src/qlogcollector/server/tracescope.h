#pragma once

#include <qstring.h>
#include <qlogcollector/comm/global.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

class QLOGCOLLECTOR_EXPORT TraceScope {
public:
    TraceScope(const QString& fileName, int line, const QString& functionSignature);
    ~TraceScope();

private:
    bool active = false;
};

QLOGCOLLECTOR_EXPORT void pushTraceFrame(const QString& fileName, int line, const QString& functionSignature);
QLOGCOLLECTOR_EXPORT void popTraceFunction();
QLOGCOLLECTOR_EXPORT QString exportTraceContext();
QLOGCOLLECTOR_EXPORT void importTraceContext(const QString& context);
QLOGCOLLECTOR_EXPORT void clearTraceContext();
QLOGCOLLECTOR_EXPORT QString currentTraceId();
QLOGCOLLECTOR_EXPORT QString currentTraceContextBase64(const QString& traceIdOverride = QString());

QLOGCOLLECTOR_END_NAMESPACE

#define QLOG_TRACE_SCOPE QLogCollector::TraceScope _qlog_trace_scope_instance_(QString::fromLatin1(__FILE__), __LINE__, QString::fromLatin1(Q_FUNC_INFO))
