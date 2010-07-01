TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_sendemailactionfactory)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

INCLUDEPATH += ../../

CONFIG += mobility
MOBILITY = contacts

DEFINES += ACTIONFACTORYPLUGINTARGET=contacts_sendemailactionfactory
DEFINES += ACTIONFACTORYPLUGINNAME=SendEmailActionFactory

HEADERS += sendemailaction_p.h
SOURCES += sendemailaction.cpp

include(../../contacts_plugins.pri)
