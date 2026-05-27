#include <qcoreapplication.h>
#include <qtimer.h>

#include "../printtest.h"
#include <qlogcollector/server/logcollector.h>

QLOGCOLLECTOR_USE_NAMESPACE

int main(int argc, char* argv[]) {

    QCoreApplication a(argc, argv);

    LogCollector::quickStart()
        .style(ROOT_PROJECT_PATH, 115)
        .console(Ide::clion)
        .fileOutput(QCoreApplication::applicationDirPath())
        .tracerOutput()
        .bindFatalSignal(true)
        .start();

    PrintTest::debugLevel();
    PrintTest::printInThread();
    PrintTest::printWithColor();
    PrintTest::longText();
    PrintTest::traceContextDemo();

    return a.exec();
}
