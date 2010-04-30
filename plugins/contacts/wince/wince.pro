TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_wince)
PLUGIN_TYPE=contacts

include(../../../common.pri)

HEADERS += \
        qcontactwincebackend_p.h \
        qcontactrequestworker_p.h \
        qcontactrequestworker.h
SOURCES += \
        qcontactwincebackend.cpp \
        contactconversions.cpp \
        qcontactrequestworker.cpp

INCLUDEPATH += $$SOURCE_DIR/src/contacts $$SOURCE_DIR/src/contacts/details $$SOURCE_DIR/src/contacts/filters $$SOURCE_DIR/src/contacts/requests

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
LIBS += pimstore.lib -lole32
CONFIG += mobility
MOBILITY = contacts
QT += network

