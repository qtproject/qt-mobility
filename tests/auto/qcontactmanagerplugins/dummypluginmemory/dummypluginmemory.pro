TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummymemory)
PLUGIN_TYPE=contacts

INCLUDEPATH += ../../../../contacts \
               ../../../../contacts/details \
               ../../../../contacts/requests \
               ../../../../contacts/filters

qtAddLibrary(QtContacts)

include(../../../../common.pri)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummymemory
DEFINES += DUMMYPLUGINNAME=memory

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
