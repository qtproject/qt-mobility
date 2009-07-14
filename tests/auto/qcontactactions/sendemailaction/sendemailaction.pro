######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testcase
TARGET = $$qtLibraryTarget(contacts_sendemailaction)

PLUGIN_SUBDIR=dummyplugin/plugins
include(../../../../common.pri)

DEFINES += ACTIONPLUGINTARGET=contacts_sendemailaction
DEFINES += ACTIONPLUGINNAME=\\\"SendEmail\\\"

INCLUDEPATH += ../../../../contacts

LIBS += -lQtContacts

HEADERS += sendemailaction_p.h
SOURCES += sendemailaction.cpp

