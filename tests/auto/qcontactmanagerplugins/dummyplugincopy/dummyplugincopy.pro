TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummycopy)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

CONFIG += mobility
MOBILITY = contacts

DEFINES += DUMMYPLUGINTARGET=contacts_testdummycopy
DEFINES += DUMMYPLUGINNAME=testdummy

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
