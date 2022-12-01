#include "logtest.h"

#include <qdebug.h>
#include <qloggingcategory.h>
#include <QtConcurrent/QtConcurrent>

Q_LOGGING_CATEGORY(mainLog, "main.log")

LogTest::LogTest(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

void LogTest::on_post_main_thread_clicked() {
    qDebug() << "post a normal log...... level d";
    qCDebug(mainLog) << "post a log with category...... level d";

    qWarning() << "post a warning log...... level w";
    qCWarning(mainLog) << "post a log with category...... level w";

    qInfo() << "post a info log...... level i";
    qCInfo(mainLog) << "post a log with category...... level i";

    qCritical() << "post a error log...... level e";
    qCCritical(mainLog) << "post a log with category...... level e";
}

void LogTest::on_post_work_thread_clicked() {
    QtConcurrent::run([&]{
        qDebug() << "post a normal log......";
        qCDebug(mainLog) << "post a log with category...... ";
    });
}
