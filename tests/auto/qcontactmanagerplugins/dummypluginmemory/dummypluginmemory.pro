######################################################################
#
# Contacts Mobility API - dummy test plugin copy (so we have two)
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummymemory)

include(../../../../common.pri)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummymemory
DEFINES += DUMMYPLUGINNAME=memory

SOURCES += ../dummyplugin/dummyplugin.cpp

# We need to link against this for all sorts of reasons :)
LIBS += -lQtContacts

include(../../contacts_plugins.pri)
