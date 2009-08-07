######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(contacts_wince)

include(../../../common.pri)

HEADERS += qcontactwincebackend_p.h
SOURCES += qcontactwincebackend.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests
    
LIBS += -lQtContacts pimstore.lib -lole32
