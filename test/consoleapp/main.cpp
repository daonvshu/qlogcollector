#include <qcoreapplication.h>
#include <qtimer.h>

#include "../printtest.h"
#include "qlogcollector.h"

#include <qdebug.h>

int main(int argc, char* argv[]) {

    QCoreApplication a(argc, argv);

#ifdef Q_OS_WIN
    logcollector::styleConfig
        .consoleApp()
        .ide_clion(false)
        //.ide_vs()
        //.ide_vscode()
        //.ide_qtcreator()
        .wordWrap(90)
        //.simpleCodeLine()
    ;
#elif defined Q_OS_LINUX
    logcollector::styleConfig
        //.consoleApp()
        //.ide_qtcreator()
        .wordWrap(90)
#endif
    logcollector::QLogCollector::init().registerLog();

    PrintTest::debugLevel();
    PrintTest::printInThread();
    PrintTest::printWithColor();
    PrintTest::longText();

    QTimer::singleShot(1000, &a, &QCoreApplication::quit);

    return a.exec();
}
