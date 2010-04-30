TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummy)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

CONFIG += mobility
MOBILITY = contacts
SOURCES += dummyplugin.cpp

DEFINES += DUMMYPLUGINNAME=testdummy

include(../../contacts_plugins.pri)
