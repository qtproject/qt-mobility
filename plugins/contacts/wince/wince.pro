TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(contacts_wince)
PLUGIN_TYPE=contacts

include(../../../common.pri)

HEADERS += qcontactwincebackend_p.h
SOURCES += qcontactwincebackend.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

qtAddLibrary(QtContacts)
LIBS += pimstore.lib -lole32

target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS+=target
