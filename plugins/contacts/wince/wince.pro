TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(contacts_wince)
PLUGIN_TYPE=contacts

include(../../../common.pri)

HEADERS += qcontactwincebackend_p.h
SOURCES += \
        qcontactwincebackend.cpp \
        contactconversions.cpp


INCLUDEPATH += $$SOURCE_DIR/src/contacts $$SOURCE_DIR/src/contacts/details $$SOURCE_DIR/src/contacts/filters $$SOURCE_DIR/src/contacts/requests

target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS+=target
LIBS += pimstore.lib -lole32
qtAddLibrary(QtContacts)

