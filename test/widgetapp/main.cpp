#include <qapplication.h>

#include "logtest.h"
#include <qlogcollector/server/logcollector.h>
#include <qlogcollector/server/outputs/fileoutputtarget.h>

QLOGCOLLECTOR_USE_NAMESPACE

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

    LogCollector::styleConfig
            .wordWrap(115)
            .projectSourceCodeRootPath(ROOT_PROJECT_PATH)
        ;
    LogCollector::addOutputTarget(OutputTarget::currentConsoleOutput(Ide::clion));
    LogCollector::addOutputTarget(new FileOutputTarget(
        FileOutputConfigBuilder().saveDir(QCoreApplication::applicationDirPath())
    ));
    LogCollector::registerLog();

    LogTest logTest;
    logTest.show();

    return a.exec();
}
