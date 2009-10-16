######################################################################
#
# Contacts Mobility API - dummy test plugin copy (so we have two)
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = contacts_testdummymemory

include(../../../../common.pri)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummymemory
DEFINES += DUMMYPLUGINNAME=memory

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
