TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(contacts_wince)
PLUGIN_TYPE=contacts

include(../../../common.pri)

HEADERS += qcontactwincebackend_p.h
SOURCES += \
        qcontactwincebackend.cpp \
        contactconversions.cpp


INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS+=target
LIBS += pimstore.lib -lole32
qtAddLibrary(QtContacts)

