######################################################################
#
# Contacts Mobility API - dummy test plugin copy (so we have two)
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testcase
TARGET = $$qtLibraryTarget(contacts_testdummyinvalid)

# Stick it somewhere else so it doesn't get picked up by default
PLUGIN_SUBDIR=dummyplugin/plugins
include(../../../../common.pri)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummyinvalid
DEFINES += DUMMYPLUGINNAME=invalid

SOURCES += ../dummyplugin/dummyplugin.cpp

# We need to link against this for all sorts of reasons :)
LIBS += -lQtContacts

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests
    
symbian{
	TARGET.EPOCALLOWDLLDATA = 1
}