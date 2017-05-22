DEFINES += EXTENSIONSYSTEM_LIBRARY
include(../../qtcreatorlibrary.pri)

unix:LIBS += $$QMAKE_LIBS_DYNLOAD

!isEmpty(vcproj) {
    DEFINES += IDE_TEST_DIR=\"$$IDE_SOURCE_TREE\"
} else {
    DEFINES += IDE_TEST_DIR=\\\"$$IDE_SOURCE_TREE\\\"
}

HEADERS += \
    invoker.h \
    iplugin.h \
    iplugin_p.h \
    extensionsystem_global.h \
    pluginmanager.h \
    pluginmanager_p.h \
    pluginspec.h \
    pluginspec_p.h 

SOURCES += \
    invoker.cpp \
    iplugin.cpp \
    pluginmanager.cpp \
    pluginspec.cpp

