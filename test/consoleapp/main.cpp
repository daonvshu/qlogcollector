#include <qcoreapplication.h>

#include "../printtest.h"
#include "qlogcollector.h"

int main(int argc, char* argv[]) {

    QCoreApplication a(argc, argv);

    logcollector::QLogCollector::init().registerLog();
    logcollector::QLogCollector::registerWin32ConsoleAppOutput();

    PrintTest::debugLevel();
    PrintTest::printInThread();
    PrintTest::printWithColor();

    return a.exec();
}