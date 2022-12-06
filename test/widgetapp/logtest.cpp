#include "logtest.h"

#include "../printtest.h"

LogTest::LogTest(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    on_post_main_thread_clicked();
    on_post_work_thread_clicked();
    on_post_styled_string_clicked();
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
