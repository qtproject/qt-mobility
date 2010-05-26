TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_booleanaction)
PLUGIN_TYPE=contacts

include(../../../../../common.pri)

INCLUDEPATH += ../../../../../src/serviceframework \
               ../../../../../src/contacts \
               ../../../../../src/contacts/details \
               ../../../../../src/contacts/requests \
               ../../../../../src/contacts/filters

INCLUDEPATH += ../../../

CONFIG += mobility
MOBILITY = contacts serviceframework

DEFINES += ACTIONPLUGINTARGET=contacts_booleanaction
DEFINES += ACTIONPLUGINNAME=BooleanAction

HEADERS += booleanaction_p.h

xml.path = $$DESTDIR/xmldata
xml.files = booleanactionservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml

include(../../../../contacts_plugins.pri)
