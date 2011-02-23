TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(versit_plugin4)
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
SOURCES += plugin4.cpp
HEADERS += plugin4.h

include(../../versit_plugins.pri)

