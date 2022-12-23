#include <qcoreapplication.h>
#include <qtimer.h>

#include "../printtest.h"
#include "qlogcollector.h"

#include <qdebug.h>

int main(int argc, char* argv[]) {

    QCoreApplication a(argc, argv);
    logcollector::QLogCollector::init().registerLog();
    logcollector::styleConfig
        .consoleApp()
        .ide_clion(true)
        //.ide_vs()
        //.ide_vscode()
        //.ide_qtcreator()
        .wordWrap(90)
        //.simpleCodeLine()
    ;

    PrintTest::debugLevel();
    PrintTest::printInThread();
    PrintTest::printWithColor();
    PrintTest::longText();

    QTimer::singleShot(1000, &a, &QCoreApplication::quit);

    return a.exec();
}
