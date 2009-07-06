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

INCLUDEPATH += ../../../contacts
    
LIBS += -lakonadi-kabc -lkpimutils -lakonadi-kde -lkabc

