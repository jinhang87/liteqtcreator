DEFINES += CORE_LIBRARY
QT += \
    help \
    network \
    printsupport \
    qml \
    sql

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(QTC_SHOW_BUILD_DATE): QTC_SHOW_BUILD_DATE = $$(QTC_SHOW_BUILD_DATE)
!isEmpty(QTC_SHOW_BUILD_DATE): DEFINES += QTC_SHOW_BUILD_DATE

include(../../qtcreatorplugin.pri)
SOURCES += \
    coreplugin.cpp

HEADERS += \
    core_global.h \
    coreplugin.h


