#include <qapplication.h>

#include "logtest.h"
#include <qlogcollector.h>
#include <qdebug.h>

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

#ifdef Q_OS_WIN
    logcollector::styleConfig
        .windowApp()
        .ide_clion(false)
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
    logcollector::QLogCollector::instance().registerLog();

    LogTest logTest;
    logTest.show();

    return a.exec();
}
