TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_serviceactionmanager)
PLUGIN_TYPE=contacts

include(../../../common.pri)

HEADERS += \
        qcontactactionservicemanager_p.h 
SOURCES += \
        qcontactactionservicemanager_p.cpp

INCLUDEPATH += $$SOURCE_DIR/src/contacts $$SOURCE_DIR/src/contacts/details $$SOURCE_DIR/src/contacts/filters $$SOURCE_DIR/src/contacts/requests $$SOURCE_DIR/src/serviceframework

CONFIG += mobility
MOBILITY = contacts serviceframework
