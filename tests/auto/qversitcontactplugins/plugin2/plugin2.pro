TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(versit_plugin2)
PLUGIN_TYPE=versit

include(../../../../common.pri)

INCLUDEPATH += \
    ../../../../src/versit \
    ../../../../src/contacts \
    ../../../../src/contacts/details \
    ../../../../src/contacts/requests \
    ../../../../src/contacts/filters

INCLUDEPATH += $$MOC_DIR

CONFIG += mobility
MOBILITY = versit
SOURCES += plugin2.cpp
HEADERS += plugin2.h

include(../../versit_plugins.pri)

