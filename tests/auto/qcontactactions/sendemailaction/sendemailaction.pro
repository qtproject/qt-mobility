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

xml.path = $$DESTDIR/xmldata
xml.files = sendemailservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml

include(../../contacts_plugins.pri)
