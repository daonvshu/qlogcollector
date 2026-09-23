#include "logthrottle.h"

QLOGCOLLECTOR_BEGIN_NAMESPACE

//Upper bound of remembered call sites, avoids unbounded growth of marked call sites
static const int MAX_CALL_SITES = 4096;

QChar logThrottleMarker() {
    return QChar(0x2063);
}

void LogThrottle::trimCallSites(qint64 nowMs, const LogThrottleConfig& config) {
    for (auto it = m_callSites.begin(); it != m_callSites.end();) {
        if (nowMs - it->lastSeenMs > config.idleResetMs) {
            it = m_callSites.erase(it);
        } else {
            ++it;
        }
    }

    while (m_callSites.size() >= MAX_CALL_SITES) {
        //drop the call site which has not been seen for the longest time
        auto oldest = m_callSites.begin();
        for (auto it = m_callSites.begin(); it != m_callSites.end(); ++it) {
            if (it->lastSeenMs < oldest->lastSeenMs) {
                oldest = it;
            }
        }
        m_callSites.erase(oldest);
    }
}

bool LogThrottle::accept(Message& message, const LogThrottleConfig& config, QString* note) {
    if (!message.log.startsWith(logThrottleMarker())) {
        return true;
    }

    //the marker is library internal information, keep it out of the log content in any case
    message.log = message.log.mid(logThrottleMarkerLength);
    if (!config.enabled) {
        return true;
    }

    const qint64 nowMs = message.timePoint;
    const qint64 baseInterval = qMax(config.baseIntervalMs, 1);
    const qint64 maxInterval = qMax<qint64>(config.maxIntervalMs, baseInterval);

    const QString callSite = message.fileName + QLatin1Char(':') + QString::number(message.codeLine);
    auto it = m_callSites.find(callSite);
    if (it == m_callSites.end()) {
        trimCallSites(nowMs, config);
        it = m_callSites.insert(callSite, CallSite());
    }

    CallSite& state = it.value();
    if (nowMs < state.lastSeenMs || nowMs - state.lastSeenMs > baseInterval) {
        //the call site has been quiet for longer than one base interval, so the previous
        //burst ended and the state goes back to the initial one, or the clock jumped
        //backwards; the messages the finished burst suppressed are reported here once with
        //the reset marker, so the first log of the new burst starts counting from zero
        if (state.suppressed > 0 && note != nullptr) {
            *note = QStringLiteral(" [throttle reset, suppressed=%1]").arg(state.suppressed);
        }
        state = CallSite();
    }
    state.lastSeenMs = nowMs;

    bool printable = false;
    if (state.emitted < config.initialBurst) {
        ++state.emitted;
        state.intervalMs = baseInterval;
        state.nextAllowedMs = nowMs + state.intervalMs;
        printable = true;
    } else if (nowMs >= state.nextAllowedMs) {
        state.intervalMs = qMin(qMax(state.intervalMs * 2, baseInterval), maxInterval);
        state.nextAllowedMs = nowMs + state.intervalMs;
        printable = true;
    } else {
        ++state.suppressed;
    }

    if (!printable) {
        return false;
    }
    if (state.suppressed > 0) {
        if (note != nullptr) {
            *note = QStringLiteral(" [suppressed=%1]").arg(state.suppressed);
        }
        state.suppressed = 0;
    }
    return true;
}

QLOGCOLLECTOR_END_NAMESPACE
