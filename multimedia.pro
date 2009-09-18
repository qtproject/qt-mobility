TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = \
    multimedia \
    plugins \
    examples \
    tests

plugins.depends = multimedia
examples.depends = multimedia
test.depends = multimedia

symbian {
 MMP_RULES += EXPORTUNFROZEN
}