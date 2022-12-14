QT       += core network

CONFIG += c++11

TARGET = server-console-test

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ./main.cpp \
    ../printtest.cpp

HEADERS += \
    ../printtest.h \

include($$PWD/../../src/src.pri)
INCLUDEPATH += ../../src
DEFINES += ROOT_PROJECT_PATH=\\\"$$PWD/../../\\\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
