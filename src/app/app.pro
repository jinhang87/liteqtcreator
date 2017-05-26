include(../../creator.pri)
#include(../shared/qtsingleapplication/qtsingleapplication.pri)

TEMPLATE = app
CONFIG += qtc_runnable sliced_bundle
TARGET = $$IDE_APP_TARGET
DESTDIR = $$IDE_APP_PATH
VERSION = $$PROJ_VERSION
QT -= testlib

SOURCES += main.cpp

include(../rpath.pri)

LIBS *= -l$$qtLibraryName(ExtensionSystem) -l$$qtLibraryName(Aggregation) -l$$qtLibraryName(Utils)

target.path = $$INSTALL_APP_PATH
INSTALLS += target

DISTFILES += \
    $$PWD/app_version.h.in

QMAKE_SUBSTITUTES += $$PWD/app_version.h.in

CONFIG += no_batch
