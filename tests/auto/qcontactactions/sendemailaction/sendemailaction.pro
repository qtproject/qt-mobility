######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testcase
TARGET = $$qtLibraryTarget(contacts_sendemailactionfactory)

PLUGIN_SUBDIR=dummyplugin/plugins
include(../../../../common.pri)

DEFINES += ACTIONFACTORYPLUGINTARGET=contacts_sendemailactionfactory
DEFINES += ACTIONFACTORYPLUGINNAME=\\\"SendEmailActionFactory\\\"

INCLUDEPATH += ../../../../contacts

LIBS += -lQtContacts

HEADERS += sendemailaction_p.h
SOURCES += sendemailaction.cpp

