TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_maliciousplugin)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../contacts \
               ../../../../contacts/details \
               ../../../../contacts/requests \
               ../../../../contacts/filters

qtAddLibrary(QtContacts)

DEFINES += MALICIOUSPLUGINTARGET=contacts_maliciousplugin
DEFINES += MALICIOUSPLUGINNAME=maliciousplugin

HEADERS += maliciousplugin_p.h
SOURCES += maliciousplugin.cpp

include(../../contacts_plugins.pri)
