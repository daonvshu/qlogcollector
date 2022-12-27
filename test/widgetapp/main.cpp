#include <qapplication.h>

#include "logtest.h"
#include <qlogcollector.h>
#include <qdebug.h>

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

    logcollector::QLogCollector::init().registerLog();
#ifdef Q_OS_WIN
    logcollector::styleConfig
        .windowApp()
        .ide_clion(true)
        //.ide_vs()
        //.ide_vscode()
        //.ide_qtcreator()
        .wordWrap(120)
        //.simpleCodeLine()
    ;
#elif defined Q_OS_LINUX
    logcollector::styleConfig
        //.windowApp()
        //.ide_qtcreator()
        .wordWrap(90)
    ;
#endif

    LogTest logTest;
    logTest.show();

    return a.exec();
}
