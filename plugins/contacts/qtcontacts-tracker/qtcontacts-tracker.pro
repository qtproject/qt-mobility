# #####################################################################
# Contacts Mobility API Tracker storage plugin
# #####################################################################

# include version number for the plugin
include(version.pri)
include(qtcontacts-tracker.pri)
include(../../../common.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_tracker)
LIBS += -L/usr/lib 

MOC_DIR = .moc/
OBJECTS_DIR = .obj/

DEPENDPATH += .
INCLUDEPATH += . 

LIBS += -lqttracker
qtAddLibrary(QtContacts)

INCLUDEPATH += $$SOURCE_DIR/contacts
INCLUDEPATH += $$SOURCE_DIR/contacts/details
INCLUDEPATH += $$SOURCE_DIR/contacts/filters
INCLUDEPATH += $$SOURCE_DIR/contacts/requests

