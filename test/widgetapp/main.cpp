#include <qapplication.h>

#include "logtest.h"
#include <qlogcollector/server/logcollector.h>

QLOGCOLLECTOR_USE_NAMESPACE

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

    LogCollector::quickStart()
        .style(ROOT_PROJECT_PATH, 115, false, false, true)
        .logThrottle(3, 300, 5000, 30000)
        .console(Ide::clion)
        .fileOutput(QCoreApplication::applicationDirPath())
        .tracerOutput()
        .start();

    LogTest logTest;
    logTest.show();

    return a.exec();
}
