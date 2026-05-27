#include <qapplication.h>

#include "logtest.h"
#include <qlogcollector/server/logcollector.h>
#include <qlogcollector/server/outputs/fileoutputtarget.h>
#include <qlogcollector/server/outputs/traceroutputtarget.h>

QLOGCOLLECTOR_USE_NAMESPACE

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

    LogCollector::styleConfig
            .wordWrap(115)
            .projectSourceCodeRootPath(ROOT_PROJECT_PATH)
            .disableNonAscii()
        ;
    LogCollector::registerLog();
    LogCollector::addOutputTarget(OutputTarget::currentConsoleOutput(Ide::clion));
    LogCollector::addOutputTarget(new FileOutputTarget(
        FileOutputConfigBuilder().saveDir(QCoreApplication::applicationDirPath())
    ));
    LogCollector::addOutputTarget(new TracerOutputTarget(
        TracerOutputConfigBuilder().saveDir(QCoreApplication::applicationDirPath())
    ));

    LogTest logTest;
    logTest.show();

    return a.exec();
}
