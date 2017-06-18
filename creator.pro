include(creator.pri)

#version check qt
!minQtVersion(5, 6, 0) {
    message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.6.0.")
}

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = i18n src

