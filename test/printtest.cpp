#include "printtest.h"

#include <qdebug.h>
#include <qloggingcategory.h>
#include <qthread.h>
#include <QtConcurrent/QtConcurrent>

#include <qlogcollector/server/colors/styledstring.h>
#include <qlogcollector/server/logcollector.h>
#include <qlogcollector/server/tracescope.h>

Q_LOGGING_CATEGORY(mainLog, "main.log")

void PrintTest::debugLevel() {
    qDebug() << "post a normal log...... level d";
    qCDebug(mainLog) << "post a log with category...... level d";

    qWarning() << "post a warning log...... level w";
    qCWarning(mainLog) << "post a log with category...... level w";

    qInfo() << "post a info log...... level i";
    qCInfo(mainLog) << "post a log with category...... level i";

    qCritical() << "post a error log...... level e 1111111111111";
    qCCritical(mainLog) << "post a log with category...... level e";
}

void PrintTest::printInThread() {
    QtConcurrent::run([&]{
        qDebug() << "post a normal log......";
        qCDebug(mainLog) << "post a log with category...... ";
        //abort();
    });
}

void PrintTest::printWithColor() {
    QtConcurrent::run([&]{
        qDebug() << "post a" + styled("color string", true, true).r().yb(1) + "log";
        qCDebug(mainLog) << "post a" << styled("color string").b(1).cb() << "log";
    });
}

void PrintTest::longText() {
    qDebug() << "this is a long" << styled("color string").b() << "text, contain more strings and" <<
        QStringLiteral("这是一个中文长字符串， 带有") << styled(QStringLiteral("部分有颜色！")).c() << QStringLiteral("的字符串。") <<
        QString("post a normal long text log").repeated(10);

    qDebug() << "fnasdjkfhbashjdfbasdfasbhjfdgahjkasdnfkasdgfbilaydfnasdjkfhbashjdfbasdfasbhjfdgahjkasdn"
                "fkasdgfbilaydfbfnasdjkfhbashjdfbasdfasbhjfdgahjkasdnfkasdgfbilaydfbfnasdjkfhbashjdfbasd"
                "fasbhjfdgahjkasdnfkasdgfbilaydfbfnasdjkfhbashjdfbasdfasbhjfdgahjkasdnfkasdgfbilaydfbfb";
    qDebug() << QString("fnasdjkfhbashjdfbasdfasbhjfdgahjkasdnfkasdgfbilaydfba").repeated(5);
}

namespace {
void traceLeaf() {
    QLOG_TRACE_SCOPE;
    qDebug() << "trace demo: leaf function log (contains trace_id, detail in trace csv)";
}

void traceMid() {
    QLOG_TRACE_SCOPE;
    traceLeaf();
}

void traceRoot() {
    QLOG_TRACE_SCOPE;
    traceMid();
}
}

void PrintTest::traceContextDemo() {
    QLOG_TRACE_SCOPE;
    qDebug() << "trace demo: root begin";
    traceRoot();

    const auto context = QLogCollector::LogCollector::exportTraceContext();
    QtConcurrent::run([context] {
        QLogCollector::LogCollector::importTraceContext(context);
        QLOG_TRACE_SCOPE;
        qDebug() << "trace demo: cross-thread continued log";
        QLogCollector::LogCollector::clearTraceContext();
    });
}

//The throttle is configured in main.cpp by Bootstrap::logThrottle().
//This call site writes the marker returned by throttlePrint() at the very beginning
//of the log statement, so it is throttled: the first 3 logs are printed, then the
//interval between two printed logs doubles from 300ms up to 5000ms, and every printed
//log carries the number of logs suppressed in between. Once the call site has been
//quiet for more than 300ms the state is back to the initial one, so the next burst
//starts over from its first 3 logs, and the logs the finished burst suppressed are
//reported once with the "throttle reset" marker. Logs from a call site without the
//marker are always printed.
void PrintTest::logThrottleDemo() {
    const int count = 300;

    qDebug() << "throttle demo: start, the marked call site below posts" << count
             << "logs in about 7 seconds, only a few of them are printed";

    QtConcurrent::run([count] {
        for (int i = 0; i < count; ++i) {
            qDebug() << throttlePrint() << "throttle demo: marked call site log" << i;
            QThread::msleep(25);
        }
        qDebug() << "throttle demo: burst finished," << count << "logs were posted by the marked call site";
    });
}
