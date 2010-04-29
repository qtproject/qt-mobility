# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
PLUGIN_TYPE=contacts

LIBS += -lqttracker
CONFIG += mobility
MOBILITY = contacts

# include version number for the plugin
include(version.pri)
include(../../../common.pri)

DEFINES += VERSION_INFO=\\\"$${VERSION_INT}\\\"

INCLUDEPATH += $$SOURCE_DIR/src/contacts
INCLUDEPATH += $$SOURCE_DIR/src/contacts/details
INCLUDEPATH += $$SOURCE_DIR/src/contacts/filters
INCLUDEPATH += $$SOURCE_DIR/src/contacts/requests

INCLUDEPATH += .

HEADERS += qcontacttrackerbackend_p.h \
           qtrackercontactasyncrequest.h \
           qtrackercontactfetchrequest.h \
           qtrackercontactslive.h \
           qtrackercontactsaverequest.h \
           qtrackerrelationshipfetchrequest.h \
           qtrackerrelationshipsaverequest.h \
           qtrackercontactidfetchrequest.h \
           trackerchangelistener.h

SOURCES += qcontacttrackerbackend.cpp \
           qtrackercontactfetchrequest.cpp \
           qtrackercontactslive.cpp \
           qtrackercontactsaverequest.cpp \
           qtrackerrelationshipfetchrequest.cpp \
           qtrackerrelationshipsaverequest.cpp \
           qtrackercontactidfetchrequest.cpp \
           trackerchangelistener.cpp

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
