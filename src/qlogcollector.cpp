#include "qlogcollector.h"

#include "ui_qlogcollector.h"

#include <qdebug.h>

QLogCollector::QLogCollector(QWidget *parent)
    : CFramelessWindow(parent) {

    ui = new Ui::QLogCollector;
    ui->setupUi(this);
}

QLogCollector::~QLogCollector() {
    delete ui;
}
