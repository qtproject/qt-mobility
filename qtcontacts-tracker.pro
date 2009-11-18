# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

# include version number for the plugin
include(version.pri)

include(qtcontacts-tracker.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
LIBS += -L/usr/lib 

MOC_DIR = .moc/
OBJECTS_DIR = .obj/

DEPENDPATH += .
INCLUDEPATH += . 

LIBS += -lqttracker

# QT += contacts
INCLUDEPATH += /usr/include/qt4/QtContacts

target.path = /usr/lib/qt4/plugins/contacts
INSTALLS += target
