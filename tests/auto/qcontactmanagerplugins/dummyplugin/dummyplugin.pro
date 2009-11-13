TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummy)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

qtAddLibrary(QtContacts)
SOURCES += dummyplugin.cpp

DEFINES += DUMMYPLUGINNAME=testdummy

include(../../contacts_plugins.pri)
