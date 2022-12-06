HEADERS += \
    ./cache.h \
    ./console.h \
    ./message.h \
    ./colorattr.h \
    ./sender.h \
    ./styledstring.h \
    ./qlogcollector.h \
    ../3rdparty/qjsonutil/include/configkey.h \
    ../3rdparty/unishox2/unishox2.h

SOURCES +=
    ./cache.cpp \
    ./console.cpp \
    ./sender.cpp \
    ./styledstring.cpp \
    ./qlogcollector.cpp \
    ../3rdparty/unishox2/unishox2.cpp

INCLUDEPATH += ../3rdparty/unishox2
INCLUDEPATH += ../3rdparty/qjsonutil/include