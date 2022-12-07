HEADERS += \
    $$PWD/cache.h \
    $$PWD/console.h \
    $$PWD/message.h \
    $$PWD/colorattr.h \
    $$PWD/sender.h \
    $$PWD/styledstring.h \
    $$PWD/qlogcollector.h \
    $$PWD/../3rdparty/qjsonutil/include/configkey.h \
    $$PWD/../3rdparty/unishox2/unishox2.h

SOURCES += \
    $$PWD/cache.cpp \
    $$PWD/console.cpp \
    $$PWD/sender.cpp \
    $$PWD/styledstring.cpp \
    $$PWD/qlogcollector.cpp \
    $$PWD/../3rdparty/unishox2/unishox2.c

INCLUDEPATH += $$PWD/../3rdparty/unishox2
INCLUDEPATH += $$PWD/../3rdparty/qjsonutil/include
