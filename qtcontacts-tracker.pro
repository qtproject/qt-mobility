# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
PLUGIN_TYPE=contacts

LIBS += -L/usr/lib
LIBS += -lqttracker
CONFIG += mobility
OBILITY = contacts

# include version number for the plugin
include(version.pri)

DEFINES += VERSION_INFO=\\\"$${VERSION_INT}\\\"

INCLUDEPATH += /usr/include/qt4/QtMobility
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

target.path = /usr/lib/qt4/plugins/contacts
INSTALLS+=target
