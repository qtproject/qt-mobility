TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummyinvalid)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

CONFIG += mobility
MOBILITY = contacts
DEFINES += DUMMYPLUGINTARGET=contacts_testdummyinvalid
DEFINES += DUMMYPLUGINNAME=invalid

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
