# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

# include version number for the plugin
include(../../version.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
LIBS += -L/usr/lib \
    -lqttracker
DEPENDPATH += .
INCLUDEPATH += . 

LIBS += -lqttracker
HEADERS += qcontacttrackerbackend_p.h \
    tracker2qcontact.h \
    qtrackercontactasyncrequest.h
SOURCES += qcontacttrackerbackend.cpp \
    tracker2qcontact.cpp \
    qtrackercontactasyncrequest.cpp

# QT += contacts
INCLUDEPATH += /usr/include/qt4/QtContacts

target.path = /usr/lib/qt4/plugins/qtcontacts-tracker
INSTALLS += target
pluginlink.path = /usr/lib/qt4/plugins/
pluginlink.uninstall = rm \
    -f \
    /usr/lib/qt4/plugins/libqtcontacts_tracker.so
pluginlink.commands = ln \
    -sf \
    qtcontacts-tracker/libqtcontacts_tracker.so \
    "\$(INSTALL_ROOT)/usr/lib/qt4/plugins/libqtcontacts_tracker.so"
INSTALLS += pluginlink

