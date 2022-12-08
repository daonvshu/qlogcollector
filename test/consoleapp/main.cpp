#include <qcoreapplication.h>
#include <qtimer.h>

#include "../printtest.h"
#include "qlogcollector.h"

int main(int argc, char* argv[]) {

    QCoreApplication a(argc, argv);

    logcollector::QLogCollector::init().registerLog();
    logcollector::styleConfig
        .target(logcollector::ConsoleOutputTarget::TARGET_STANDARD_OUTPUT)
        .simpleCodeLine()
        .lineWidth(66);

    PrintTest::debugLevel();
    PrintTest::printInThread();
    PrintTest::printWithColor();
    PrintTest::longText();

    QTimer::singleShot(1000, &a, &QCoreApplication::quit);

    return a.exec();
}