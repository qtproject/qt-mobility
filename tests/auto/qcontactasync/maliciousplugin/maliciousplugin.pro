TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_maliciousplugin)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

INCLUDEPATH += ../../

CONFIG += mobility
MOBILITY = contacts

DEFINES += MALICIOUSPLUGINTARGET=contacts_maliciousplugin
DEFINES += MALICIOUSPLUGINNAME=maliciousplugin

HEADERS += maliciousplugin_p.h
SOURCES += maliciousplugin.cpp

include(../../contacts_plugins.pri)
