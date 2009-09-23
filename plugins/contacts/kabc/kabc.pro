######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(contacts_kabc)

include(../../../common.pri)

HEADERS += qcontactkabcbackend_p.h
SOURCES += qcontactkabcbackend.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests
    
LIBS += -lakonadi-kabc -lkpimutils -lakonadi-kde -lkabc

target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS+=target
