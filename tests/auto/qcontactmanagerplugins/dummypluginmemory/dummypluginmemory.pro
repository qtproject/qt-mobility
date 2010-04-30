TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummymemory)
PLUGIN_TYPE=contacts

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

CONFIG += mobility
MOBILITY = contacts

include(../../../../common.pri)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummymemory
DEFINES += DUMMYPLUGINNAME=memory

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
