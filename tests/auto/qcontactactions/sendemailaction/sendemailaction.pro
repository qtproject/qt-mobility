######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = contacts_sendemailactionfactory

include(../../../../common.pri)
INCLUDEPATH += $$SOURCE_DIR/tests/auto
DEFINES += ACTIONFACTORYPLUGINTARGET=contacts_sendemailactionfactory
DEFINES += ACTIONFACTORYPLUGINNAME=SendEmailActionFactory

HEADERS += sendemailaction_p.h
SOURCES += sendemailaction.cpp

include(../../contacts_plugins.pri)
