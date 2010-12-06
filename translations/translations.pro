include(../staticconfig.pri)

# most of this is shamelessly copied from Qt Creator

LANGUAGES =

LUPDATE_ARGS = \
    -I../include \
    src/bearer \
    src/contacts \
    src/location \
    src/messaging \
    src/multimedia \
    src/publishsubscribe \
    src/sensors \
    src/serviceframework \
    src/systeminfo \
    src/versit \
    plugins

# var, prepend, append
defineReplace(prependAll) {
    for(a,$$1):result += $$2$${a}$$3
    return($$result)
}

qtPrepareTool(LUPDATE, lupdate)
qtPrepareTool(LRELEASE, lrelease)
qtPrepareTool(LCONVERT, lconvert)
LUPDATE += -locations relative -no-ui-lines -no-sort

TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/qtmobility_,.ts)

files = $$files($$PWD/*_??.ts) $$PWD/qtmobility_untranslated.ts
for(file, files) {
    lang = $$replace(file, .*_([^/]*)\\.ts, \\1)
    v = ts-$${lang}.commands
    $$v = cd $$QT_MOBILITY_SOURCE_TREE && $$LUPDATE $$LUPDATE_ARGS -ts $$file
    QMAKE_EXTRA_TARGETS += ts-$$lang
}
ts-all.commands = cd $$QT_MOBILITY_SOURCE_TREE && $$LUPDATE $$LUPDATE_ARGS -ts $$files
QMAKE_EXTRA_TARGETS += ts-all

check-ts.commands = (cd $$PWD && perl check-ts.pl)
check-ts.depends = ts-all
QMAKE_EXTRA_TARGETS += check-ts

isEqual(QMAKE_DIR_SEP, /) {
    commit-ts.commands = \
        cd $$IDE_SOURCE_TREE; \
        for f in `git diff-files --name-only translations/*_??.ts`; do \
            $$LCONVERT -locations none -i \$\$f -o \$\$f; \
        done; \
        git add translations/*_??.ts && git commit
} else {
    wd = $$replace(IDE_SOURCE_TREE, /, \\)
    commit-ts.commands = \
        cd $$wd && \
        for /f usebackq %%f in (`git diff-files --name-only translations/*_??.ts`) do \
            $$LCONVERT -locations none -i %%f -o %%f $$escape_expand(\\n\\t) \
        cd $$wd && git add translations/*_??.ts && git commit
}
QMAKE_EXTRA_TARGETS += commit-ts

TEMPLATE = app
TARGET = qm_phony_target
CONFIG -= qt separate_debug_info gdb_dwarf_index sis_targets
CONFIG += no_icon
QT =
LIBS =

contains(TEMPLATE_PREFIX, vc):vcproj = 1

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
silent:updateqm.commands = @echo lrelease ${QMAKE_FILE_IN} && $$updateqm.commands
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

isEmpty(vcproj) {
    QMAKE_LINK = @: IGNORE THIS LINE
    OBJECTS_DIR =
    win32:CONFIG -= embed_manifest_exe
} else {
    CONFIG += console
    PHONY_DEPS = .
    phony_src.input = PHONY_DEPS
    phony_src.output = phony.c
    phony_src.variable_out = GENERATED_SOURCES
    phony_src.commands = echo int main() { return 0; } > phony.c
    phony_src.name = CREATE phony.c
    phony_src.CONFIG += combine
    QMAKE_EXTRA_COMPILERS += phony_src
}

qmfiles.files = $$prependAll(LANGUAGES, $$OUT_PWD/qtmobility_,.qm)
meego|maemo6 {
    qmfiles.path = $${QT_MOBILITY_PREFIX}/share/qtmobility/translations
} else {
    qmfiles.path = $${QT_MOBILITY_PREFIX}/translations
}
qmfiles.CONFIG += no_check_exist
INSTALLS += qmfiles
