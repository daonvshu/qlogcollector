#include <qcoreapplication.h>
#include <qtimer.h>

#include "../printtest.h"
#include <qlogcollector/server/logcollector.h>
#include <qlogcollector/server/outputs/fileoutputtarget.h>
#include <qlogcollector/server/outputs/traceroutputtarget.h>

QLOGCOLLECTOR_USE_NAMESPACE

int main(int argc, char* argv[]) {

    QCoreApplication a(argc, argv);

    LogCollector::styleConfig
        .wordWrap(115)
        .projectSourceCodeRootPath(ROOT_PROJECT_PATH)
    ;
    LogCollector::registerLog();
    LogCollector::addOutputTarget(OutputTarget::currentConsoleOutput(Ide::clion));
    LogCollector::addOutputTarget(new FileOutputTarget(
        FileOutputConfigBuilder().saveDir(QCoreApplication::applicationDirPath())
    ));
    LogCollector::addOutputTarget(new TracerOutputTarget(
        TracerOutputConfigBuilder().saveDir(QCoreApplication::applicationDirPath())
    ));
    LogCollector::bindSignalFatal();

    PrintTest::debugLevel();
    PrintTest::printInThread();
    PrintTest::printWithColor();
    PrintTest::longText();
    PrintTest::traceContextDemo();

    return a.exec();
}
