#pragma once

#include <qchar.h>
#include <qglobal.h>
#include <qhash.h>
#include <qstring.h>

#include <qlogcollector/comm/global.h>
#include <qlogcollector/comm/message.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

//Out-of-band marker of the call site throttle, used in the same way as
//allowNonAsciiPrint() with 0x2060: writing it at the beginning of a log
//statement enables throttling for that call site.
QLOGCOLLECTOR_EXPORT QChar logThrottleMarker();

//Length of the marker: the marker itself plus the separator space inserted by QDebug
constexpr int logThrottleMarkerLength = 2;

//Log throttle parameters, all durations are in milliseconds
struct LogThrottleConfig {
    bool enabled = true;            //disabled means the marker is ignored and logs keep their frequency
    int initialBurst = 3;           //messages printed continuously at the beginning of a burst
    int baseIntervalMs = 1000;      //gap between the first two backed off messages, and the quiet
                                    //time after which the next message starts a new burst
    int maxIntervalMs = 30000;      //upper bound of the gap
    int idleResetMs = 90000;        //idle time after which the record of a call site is dropped
};

//Throttle marked call sites with exponential backoff, the number of suppressed
//messages is added to the next printed line right after its content. A call site
//goes back to its initial state once it has been quiet for longer than
//baseIntervalMs, so every new burst is printed from its beginning. The state is
//only used from the log thread, so no lock is needed.
class QLOGCOLLECTOR_EXPORT LogThrottle {
public:
    //Decide whether the message is printed this time, the marker is always stripped.
    //The number of messages suppressed since the last printed one is written to note
    //and put on that line by the caller.
    //When the state of a call site goes back to the initial one, the messages suppressed
    //by the finished burst are reported by note once with the reset marker, so they are
    //not mixed up with the count of the new burst.
    //Returns false when the message is suppressed.
    bool accept(Message& message, const LogThrottleConfig& config, QString* note = nullptr);

private:
    struct CallSite {
        int emitted = 0;
        int suppressed = 0;
        qint64 intervalMs = 0;
        qint64 nextAllowedMs = 0;
        qint64 lastSeenMs = 0;
    };

    void trimCallSites(qint64 nowMs, const LogThrottleConfig& config);

private:
    QHash<QString, CallSite> m_callSites;
};

QLOGCOLLECTOR_END_NAMESPACE
