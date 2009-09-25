TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummy)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../contacts \
               ../../../../contacts/details \
               ../../../../contacts/requests \
               ../../../../contacts/filters

qtAddLibrary(QtContacts)
SOURCES += dummyplugin.cpp

DEFINES += DUMMYPLUGINNAME=testdummy

include(../../contacts_plugins.pri)
