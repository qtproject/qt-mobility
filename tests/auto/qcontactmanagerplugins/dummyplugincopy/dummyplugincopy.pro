######################################################################
#
# Contacts Mobility API - dummy test plugin copy (so we have two)
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummycopy)

include(../../../../common.pri)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummycopy
DEFINES += DUMMYPLUGINNAME=testdummy

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
