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

LIBS += -lqttracker
CONFIG += mobility
MOBILITY = contacts

target.path = $$SOURCE_DIR/plugins/contacts
INSTALLS += target
