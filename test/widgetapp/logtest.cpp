#include "logtest.h"

#include "../printtest.h"

#include <qsysinfo.h>
#include <qdebug.h>

LogTest::LogTest(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    on_post_main_thread_clicked();
    on_post_work_thread_clicked();
    on_post_styled_string_clicked();
    on_post_long_string_clicked();
}

void LogTest::on_post_main_thread_clicked() {
    PrintTest::debugLevel();
}

void LogTest::on_post_work_thread_clicked() {
    PrintTest::printInThread();
}

void LogTest::on_post_styled_string_clicked() {
    PrintTest::printWithColor();
}

void LogTest::on_post_long_string_clicked() {
    PrintTest::longText();

    qDebug() << QSysInfo::bootUniqueId();
    qDebug() << QSysInfo::buildAbi();
    qDebug() << QSysInfo::buildCpuArchitecture();
    qDebug() << QSysInfo::currentCpuArchitecture();
    qDebug() << QSysInfo::kernelType();
    qDebug() << QSysInfo::kernelVersion();
    qDebug() << QSysInfo::machineHostName();
    qDebug() << QSysInfo::machineUniqueId();
    qDebug() << QSysInfo::prettyProductName();
    qDebug() << QSysInfo::productType();
    qDebug() << QSysInfo::productVersion();

    QString sysInfo = "%1/%2 %3 %4 (%5)";
    sysInfo = sysInfo.arg(QSysInfo::prettyProductName(), QSysInfo::kernelType(), QSysInfo::kernelVersion(),
                          QSysInfo::currentCpuArchitecture(), QSysInfo::machineHostName());
    qDebug() << sysInfo;
}
