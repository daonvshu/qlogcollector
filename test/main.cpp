#include <qapplication.h>

#include "logtest.h"
#include <qlogcollector.h>

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

    logcollector::QLogCollector::init().registerLog();

    LogTest logTest;
    logTest.show();

    return a.exec();
}