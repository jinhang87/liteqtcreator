TEMPLATE = aux

include(../creator.pri)
!build_pass:message(IDE_OUTPUT_PATH: $$IDE_OUTPUT_PATH)
!build_pass:message(IDE_SOURCE_TREE: $$IDE_SOURCE_TREE)
!build_pass:message(IDE_BIN_PATH: $$IDE_BIN_PATH)
!build_pass:message(INSTALL_APP_PATH: $$INSTALL_APP_PATH)
!build_pass:message(INSTALL_DATA_PATH: $$INSTALL_DATA_PATH)

#LANGUAGES = cs de fr ja pl ru sl uk zh_CN zh_TW
LANGUAGES = zh_CN

# *don't* re-enable these without a prior rework
BAD_LANGUAGES = hu

# var, prepend, append
defineReplace(prependAll) {
    for(a,$$1):result += $$2$${a}$$3
    return($$result)
}

LUPDATE = $$shell_path($$[QT_INSTALL_BINS]/lupdate) -locations relative -no-ui-lines -no-sort
LRELEASE = $$shell_path($$[QT_INSTALL_BINS]/lrelease)
LCONVERT = $$shell_path($$[QT_INSTALL_BINS]/lconvert)
!build_pass:message(LUPDATE : $$LUPDATE)
!build_pass:message(LRELEASE : $$LRELEASE)
!build_pass:message(LCONVERT : $$LCONVERT)

wd = $$replace(IDE_SOURCE_TREE, /, $$QMAKE_DIR_SEP)
!build_pass:message(wd: $$wd)

TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/,.ts)
!build_pass:message(TRANSLATIONS: $$TRANSLATIONS)

plugin_sources = $$files($$IDE_SOURCE_TREE/src/plugins/*)
plugin_sources ~= s,^$$re_escape($$IDE_SOURCE_TREE/),,g$$i_flag
plugin_sources -= src/plugins/plugins.pro 
sources = src/app src/libs
!build_pass:message(plugin_sources: $$plugin_sources)
!build_pass:message(sources: $$sources)

for(path, INCLUDEPATH): include_options *= -I$$shell_quote($$path)
files = $$files($$PWD/*_??.ts) $$PWD/untranslated.ts
for(file, files) {
    lang = $$replace(file, .*_([^/]*)\\.ts, \\1)
    v = ts-$${lang}.commands
    $$v = cd $$wd && $$LUPDATE $$include_options $$sources -ts $$file
    v = ts-$${lang}.depends
    $$v = extract
    QMAKE_EXTRA_TARGETS += ts-$$lang
}
ts-all.commands = cd $$wd && $$LUPDATE $$include_options $$sources -ts $$files
QMAKE_EXTRA_TARGETS += ts-all
!build_pass:message(files: $$files)

updateqm.input = TRANSLATIONS
updateqm.output = $$IDE_DATA_PATH/translations/${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
!build_pass:message(QMAKE_FILE_IN: $${QMAKE_FILE_IN})
!build_pass:message(QMAKE_FILE_OUT: $${QMAKE_FILE_OUT})

qmfiles.files = $$prependAll(LANGUAGES, $$IDE_DATA_PATH/translations/,.qm)
qmfiles.path = $$INSTALL_DATA_PATH/translations
!build_pass:message(path: $$qmfiles.path)
qmfiles.CONFIG += no_check_exist
INSTALLS += qmfiles
!build_pass:message(INSTALLS: $$INSTALLS)
