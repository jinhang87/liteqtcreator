DEFINES += CORE_LIBRARY

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(QTC_SHOW_BUILD_DATE): QTC_SHOW_BUILD_DATE = $$(QTC_SHOW_BUILD_DATE)
!isEmpty(QTC_SHOW_BUILD_DATE): DEFINES += QTC_SHOW_BUILD_DATE

include(../../plugin.pri)
SOURCES += \
    coreplugin.cpp \
    mainmenu.cpp \
    imenuiconfactory.cpp

HEADERS += \
    core_global.h \
    coreplugin.h \
    mainmenu.h \
    imenuiconfactory.h

FORMS += \
    mainmenu.ui


