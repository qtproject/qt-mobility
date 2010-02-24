# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

# include version number for the plugin
TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
PLUGIN_TYPE=contacts

LIBS += -lqttracker
CONFIG += mobility
MOBILITY = contacts

include(version.pri)
include(qtcontacts-tracker.pri)

LIBS += -L/usr/lib 

MOC_DIR = .moc/
OBJECTS_DIR = .obj/

DEPENDPATH += .
INCLUDEPATH += . 
INCLUDEPATH += /usr/include 

LIBS += -lqttracker

# QT += contacts
INCLUDEPATH += /usr/include/qt4/QtMobility

target.path = /usr/lib/qt4/plugins/contacts
INSTALLS += target
