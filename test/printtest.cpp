#include "printtest.h"

#include <qdebug.h>
#include <qloggingcategory.h>
#include <QtConcurrent/QtConcurrent>

#include "styledstring.h"

Q_LOGGING_CATEGORY(mainLog, "main.log")

void PrintTest::debugLevel() {
    qDebug() << "post a normal log...... level d";
    qCDebug(mainLog) << "post a log with category...... level d";

    qWarning() << "post a warning log...... level w";
    qCWarning(mainLog) << "post a log with category...... level w";

    qInfo() << "post a info log...... level i";
    qCInfo(mainLog) << "post a log with category...... level i";

    qCritical() << "post a error log...... level e";
    qCCritical(mainLog) << "post a log with category...... level e";
}

void PrintTest::printInThread() {
    QtConcurrent::run([&]{
        qDebug() << "post a normal log......";
        qCDebug(mainLog) << "post a log with category...... ";
    });
}

void PrintTest::printWithColor() {
    QtConcurrent::run([&]{
        qDebug() << "post a" + styled("color string", true, true).r().yb(1) + "log";
        qCDebug(mainLog) << "post a" << styled("color string").b(1).cb() << "log";
    });
}
