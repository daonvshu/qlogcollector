#pragma once

#include "ui_logtest.h"

class LogTest : public QWidget {
    Q_OBJECT

public:
    explicit LogTest(QWidget* parent = nullptr);

private:
    Ui::LogTest ui;

private slots:
    void on_post_main_thread_clicked();
    void on_post_work_thread_clicked();
};
