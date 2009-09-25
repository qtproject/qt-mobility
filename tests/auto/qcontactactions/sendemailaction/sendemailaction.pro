TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_sendemailactionfactory)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

INCLUDEPATH += ../../../../contacts \
               ../../../../contacts/details \
               ../../../../contacts/requests \
               ../../../../contacts/filters

qtAddLibrary(QtContacts)

DEFINES += ACTIONFACTORYPLUGINTARGET=contacts_sendemailactionfactory
DEFINES += ACTIONFACTORYPLUGINNAME=SendEmailActionFactory

HEADERS += sendemailaction_p.h
SOURCES += sendemailaction.cpp

include(../../contacts_plugins.pri)
