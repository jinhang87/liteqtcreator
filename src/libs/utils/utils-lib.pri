dll {
    DEFINES += QTCREATOR_UTILS_LIB
} else {
    DEFINES += QTCREATOR_UTILS_STATIC_LIB
}

#QT += gui network qml

#CONFIG += exceptions # used by portlist.cpp, textfileformat.cpp, and ssh/*

#win32: LIBS += -luser32 -lshell32
# PortsGatherer
#win32: LIBS += -liphlpapi -lws2_32

SOURCES += \
    $$PWD/qtcassert.cpp

HEADERS += \
    $$PWD/qtcassert.h \
    $$PWD/algorithm.h



