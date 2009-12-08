# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
PLUGIN_TYPE=contacts

LIBS += -lqttracker
qtAddLibrary(QtContacts)

# include version number for the plugin
include(version.pri)
include(../../../common.pri)

INCLUDEPATH += $$SOURCE_DIR/src/contacts
INCLUDEPATH += $$SOURCE_DIR/src/contacts/details
INCLUDEPATH += $$SOURCE_DIR/src/contacts/filters
INCLUDEPATH += $$SOURCE_DIR/src/contacts/requests

INCLUDEPATH += .

HEADERS += qcontacttrackerbackend_p.h \
           qtrackercontactasyncrequest.h \
           trackerchangelistener.h \
           qtrackercontactslive.h \
           qtrackercontactsaverequest.h \
           qtrackerrelationshipfetchrequest.h \
           qtrackerrelationshipsaverequest.h \
           qtrackercontactidfetchrequest.h

SOURCES += qcontacttrackerbackend.cpp \
           qtrackercontactasyncrequest.cpp \
           trackerchangelistener.cpp \
           qtrackercontactslive.cpp \
           qtrackercontactsaverequest.cpp \
           qtrackerrelationshipfetchrequest.cpp \
           qtrackerrelationshipsaverequest.cpp \
           qtrackercontactidfetchrequest.cpp

target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS+=target
