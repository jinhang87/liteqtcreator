!isEmpty(CREATOR_PRI_INCLUDED):error("creator.pri already included")
CREATOR_PRI_INCLUDED = 1

PROJ_VERSION = 4.2.1
PROJ_COMPAT_VERSION = 4.2.0
VERSION = $$PROJ_VERSION
BINARY_ARTIFACTS_BRANCH = 4.2

defineReplace(qtLibraryTargetName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

defineReplace(qtLibraryName) {
   RET = $$qtLibraryTargetName($$1)
   win32 {
      VERSION_LIST = $$split(PROJ_VERSION, .)
      RET = $$RET$$first(VERSION_LIST)
   }
   return($$RET)
}

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

# For use in custom compilers which just copy files
defineReplace(stripSrcDir) {
    return($$relative_path($$absolute_path($$1, $$OUT_PWD), $$_PRO_FILE_PWD_))
}

isEmpty(IDE_LIBRARY_BASENAME) {
    IDE_LIBRARY_BASENAME = lib
}

IDE_SOURCE_TREE = $$PWD
isEmpty(IDE_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    IDE_BUILD_TREE = $$clean_path($$OUT_PWD)
    IDE_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}

IDE_APP_PATH = $$IDE_BUILD_TREE/bin
!osx {
    contains(TEMPLATE, vc.*):vcproj = 1
    IDE_APP_TARGET   = creator

    # target output path if not set manually
    isEmpty(IDE_OUTPUT_PATH): IDE_OUTPUT_PATH = $$IDE_BUILD_TREE
    isEmpty(QTC_PREFIX): QTC_PREFIX = $$IDE_BUILD_TREE/install

    IDE_LIBRARY_PATH = $$IDE_OUTPUT_PATH/$$IDE_LIBRARY_BASENAME/$$IDE_APP_TARGET
    IDE_PLUGIN_PATH  = $$IDE_LIBRARY_PATH/plugins
    IDE_DATA_PATH    = $$IDE_OUTPUT_PATH/share/$$IDE_APP_TARGET
    IDE_DOC_PATH     = $$IDE_OUTPUT_PATH/share/doc/$$IDE_APP_TARGET
    IDE_BIN_PATH     = $$IDE_OUTPUT_PATH/bin
    win32: \
        IDE_LIBEXEC_PATH = $$IDE_OUTPUT_PATH/bin
    else: \
        IDE_LIBEXEC_PATH = $$IDE_OUTPUT_PATH/libexec/$$IDE_APP_TARGET
    !isEqual(IDE_SOURCE_TREE, $$IDE_OUTPUT_PATH):copydata = 1

    LINK_LIBRARY_PATH = $$IDE_BUILD_TREE/$$IDE_LIBRARY_BASENAME/$$IDE_APP_TARGET
    LINK_PLUGIN_PATH  = $$LINK_LIBRARY_PATH/plugins

    INSTALL_LIBRARY_PATH = $$QTC_PREFIX/$$IDE_LIBRARY_BASENAME/$$IDE_APP_TARGET
    INSTALL_PLUGIN_PATH  = $$INSTALL_LIBRARY_PATH/plugins
    win32: \
        INSTALL_LIBEXEC_PATH = $$QTC_PREFIX/bin
    else: \
        INSTALL_LIBEXEC_PATH = $$QTC_PREFIX/libexec/$$IDE_APP_TARGET
    INSTALL_DATA_PATH    = $$QTC_PREFIX/share/$$IDE_APP_TARGET
    INSTALL_DOC_PATH     = $$QTC_PREFIX/share/doc/$$IDE_APP_TARGET
    INSTALL_BIN_PATH     = $$QTC_PREFIX/bin
    INSTALL_APP_PATH     = $$QTC_PREFIX/bin
}

INCLUDEPATH += \
    $$IDE_BUILD_TREE/src \ # for <app/app_version.h> in case of actual build directory
    $$IDE_SOURCE_TREE/src \ # for <app/app_version.h> in case of binary package with dev package
    $$IDE_SOURCE_TREE/src/libs \
    $$IDE_SOURCE_TREE/tools

win32:exists($$IDE_SOURCE_TREE/lib/$$IDE_APP_TARGET) {
    # for .lib in case of binary package with dev package
    LIBS *= -L$$IDE_SOURCE_TREE/lib/$$IDE_APP_TARGET
    LIBS *= -L$$IDE_SOURCE_TREE/lib/$$IDE_APP_TARGET/plugins
}

QTC_PLUGIN_DIRS_FROM_ENVIRONMENT = $$(QTC_PLUGIN_DIRS)
QTC_PLUGIN_DIRS += $$split(QTC_PLUGIN_DIRS_FROM_ENVIRONMENT, $$QMAKE_DIRLIST_SEP)
QTC_PLUGIN_DIRS += $$IDE_SOURCE_TREE/src/plugins
for(dir, QTC_PLUGIN_DIRS) {
    INCLUDEPATH += $$dir
}

CONFIG += \
    depend_includepath \
    no_include_pwd

LIBS *= -L$$LINK_LIBRARY_PATH  # Qt Creator libraries
exists($$IDE_LIBRARY_PATH): LIBS *= -L$$IDE_LIBRARY_PATH  # library path from output path

!isEmpty(vcproj) {
    DEFINES += IDE_LIBRARY_BASENAME=\"$$IDE_LIBRARY_BASENAME\"
} else {
    DEFINES += IDE_LIBRARY_BASENAME=\\\"$$IDE_LIBRARY_BASENAME\\\"
}

DEFINES += \
    QT_NO_CAST_TO_ASCII \
    QT_RESTRICTED_CAST_FROM_ASCII \
    QT_DISABLE_DEPRECATED_BEFORE=0x050600
!macx:DEFINES += QT_USE_FAST_OPERATOR_PLUS QT_USE_FAST_CONCATENATION

unix {
    CONFIG(debug, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/debug-shared
    CONFIG(release, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/release-shared

    CONFIG(debug, debug|release):MOC_DIR = $${OUT_PWD}/.moc/debug-shared
    CONFIG(release, debug|release):MOC_DIR = $${OUT_PWD}/.moc/release-shared

    RCC_DIR = $${OUT_PWD}/.rcc
    UI_DIR = $${OUT_PWD}/.uic
}

msvc {
    #Don't warn about sprintf, fopen etc being 'unsafe'
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CXXFLAGS_WARN_ON *= -w44996
    # Speed up startup time when debugging with cdb
    QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
}

qt {
    contains(QT, core): QT += concurrent
    contains(QT, gui): QT += widgets
}

QBSFILE = $$replace(_PRO_FILE_, \\.pro$, .qbs)
exists($$QBSFILE):DISTFILES += $$QBSFILE

!isEmpty(QTC_PLUGIN_DEPENDS) {
    LIBS *= -L$$IDE_PLUGIN_PATH  # plugin path from output directory
    LIBS *= -L$$LINK_PLUGIN_PATH  # when output path is different from Qt Creator build directory
}

# recursively resolve plugin deps
done_plugins =
for(ever) {
    isEmpty(QTC_PLUGIN_DEPENDS): \
        break()
    done_plugins += $$QTC_PLUGIN_DEPENDS
    for(dep, QTC_PLUGIN_DEPENDS) {
        dependencies_file =
        for(dir, QTC_PLUGIN_DIRS) {
            exists($$dir/$$dep/$${dep}_dependencies.pri) {
                dependencies_file = $$dir/$$dep/$${dep}_dependencies.pri
                break()
            }
        }
        isEmpty(dependencies_file): \
            error("Plugin dependency $$dep not found")
        include($$dependencies_file)
        LIBS += -l$$qtLibraryName($$QTC_PLUGIN_NAME)
    }
    QTC_PLUGIN_DEPENDS = $$unique(QTC_PLUGIN_DEPENDS)
    QTC_PLUGIN_DEPENDS -= $$unique(done_plugins)
}

# recursively resolve library deps
done_libs =
for(ever) {
    isEmpty(QTC_LIB_DEPENDS): \
        break()
    done_libs += $$QTC_LIB_DEPENDS
    for(dep, QTC_LIB_DEPENDS) {
        include($$PWD/src/libs/$$dep/$${dep}_dependencies.pri)
        LIBS += -l$$qtLibraryName($$QTC_LIB_NAME)
    }
    QTC_LIB_DEPENDS = $$unique(QTC_LIB_DEPENDS)
    QTC_LIB_DEPENDS -= $$unique(done_libs)
}

