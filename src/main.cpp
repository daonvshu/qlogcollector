#include <qapplication.h>

#include "qlogcollector.h"

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

    QLogCollector logCollector;
    logCollector.show();

    return a.exec();
}