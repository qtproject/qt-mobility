######################################################################
#
# Contacts Mobility API - dummy test plugin
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testcase
TARGET = $$qtLibraryTarget(contacts_testdummy)

# Stick it somewhere else so it doesn't get picked up by default
PLUGIN_SUBDIR=dummyplugin/plugins
include(../../../../common.pri)

SOURCES += dummyplugin.cpp

# We need to link against this for all sorts of reasons :)
LIBS += -lQtContacts

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests
    
