TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummyempty)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../contacts \
               ../../../../contacts/details \
               ../../../../contacts/requests \
               ../../../../contacts/filters

qtAddLibrary(QtContacts)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummyempty

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
