######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_wince)

include(../../../common.pri)

HEADERS += qcontactwincebackend_p.h
SOURCES += \
        qcontactwincebackend.cpp \
        contactconversions.cpp


INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += pimstore.lib -lole32

qtAddLibrary(QtContacts)

