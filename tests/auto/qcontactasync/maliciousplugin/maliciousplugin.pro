######################################################################
#
# Contacts Mobility API - malicious test plugin
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = contacts_maliciousplugin

include(../../../../common.pri)
INCLUDEPATH += $$SOURCE_DIR/tests/auto
DEFINES += MALICIOUSPLUGINTARGET=contacts_maliciousplugin
DEFINES += MALICIOUSPLUGINNAME=maliciousplugin

HEADERS += maliciousplugin_p.h
SOURCES += maliciousplugin.cpp

include(../../contacts_plugins.pri)
