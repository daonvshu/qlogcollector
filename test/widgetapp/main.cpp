#include <qapplication.h>

#include "logtest.h"
#include <qlogcollector.h>
#include <qdebug.h>

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

    logcollector::QLogCollector::init().registerLog();
    logcollector::styleConfig
        .windowApp()
        .ide_clion(false)
        //.ide_vs()
        //.ide_vscode()
        //.ide_qtcreator()
        .wordWrap(120)
        //.simpleCodeLine()
    ;

    LogTest logTest;
    logTest.show();

    return a.exec();
}