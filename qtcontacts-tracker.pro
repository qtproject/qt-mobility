# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

# include version number for the plugin
include(../../version.pri)

include(../../common/commonfilters.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
LIBS += -L/usr/lib \
    -lqttracker
DEPENDPATH += .
INCLUDEPATH += . 

LIBS += -lqttracker
HEADERS += qcontacttrackerbackend_p.h \
    qtrackercontactasyncrequest.h \
    trackerchangelistener.h
SOURCES += qcontacttrackerbackend.cpp \
    qtrackercontactasyncrequest.cpp \
    trackerchangelistener.cpp

# QT += contacts
INCLUDEPATH += /usr/include/qt4/QtContacts

target.path = /usr/lib/qt4/plugins/contacts
INSTALLS += target
