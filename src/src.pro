include(../qtcreator.pri)

TEMPLATE  = subdirs
CONFIG   += ordered

exists(shared/qbs/qbs.pro) {
    isEmpty(QBS_INSTALL_DIR):QBS_INSTALL_DIR = $$(QBS_INSTALL_DIR)
    isEmpty(QBS_INSTALL_DIR):SUBDIRS += $$QBS_DIRS
}

SUBDIRS += \
    libs \
    app
