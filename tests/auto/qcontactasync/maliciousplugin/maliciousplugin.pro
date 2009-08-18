######################################################################
#
# Contacts Mobility API - malicious test plugin
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testcase
TARGET = $$qtLibraryTarget(contacts_maliciousplugin)

include(../../../../common.pri)

SOURCES += maliciousplugin.cpp

DEFINES += MALICIOUSPLUGINNAME=maliciousplugin

# We need to link against this for all sorts of reasons :)
LIBS += -lQtContacts

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

include(../../contacts_plugins.pri)
