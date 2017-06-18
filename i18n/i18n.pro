TEMPLATE = aux

include(../creator.pri)

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

wd = $$replace(IDE_SOURCE_TREE, /, $$QMAKE_DIR_SEP)
TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/,.ts)
QM_DIR_NAME = i18n
plugin_sources = $$files($$IDE_SOURCE_TREE/src/plugins/*)
plugin_sources ~= s,^$$re_escape($$IDE_SOURCE_TREE/),,g$$i_flag
plugin_sources -= src/plugins/plugins.pro 
sources = src/app src/libs

for(path, INCLUDEPATH): include_options *= -I$$shell_quote($$path)
#files = $$files($$PWD/*_??.ts)
files = $$TRANSLATIONS

#for(file, files) {
#    lang = $$replace(file, .*_([^/]*)\\.ts, \\1)
#    v = ts-$${lang}.commands
#    $$v = cd $$wd && $$LUPDATE $$include_options $$sources -ts $$file
#    !build_pass:message(v: $$v)
#    !build_pass:message(commands: cd $$wd && $$LUPDATE $$include_options $$sources -ts $$file)
#    v = ts-$${lang}.depends
#    $$v = extract
#    QMAKE_EXTRA_TARGETS += ts-$$lang
#}

ts-all.commands = cd $$wd && $$LUPDATE $$include_options $$sources $$plugin_sources -ts $$files
!build_pass:message(commands: cd $$wd && $$LUPDATE $$include_options $$sources $$plugin_sources -ts $$files)
QMAKE_EXTRA_TARGETS += ts-all

updateqm.input = TRANSLATIONS
updateqm.output = $$IDE_DATA_PATH/$$QM_DIR_NAME/${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

qmfiles.files = $$prependAll(LANGUAGES, $$IDE_DATA_PATH/$$QM_DIR_NAME/,.qm)
qmfiles.path = $$INSTALL_DATA_PATH/$$QM_DIR_NAME
qmfiles.CONFIG += no_check_exist
INSTALLS += qmfiles
