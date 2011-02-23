TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(versit_plugin5)
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
SOURCES += plugin5.cpp
HEADERS += plugin5.h

include(../../versit_plugins.pri)

