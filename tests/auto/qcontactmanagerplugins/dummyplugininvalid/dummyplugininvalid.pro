######################################################################
#
# Contacts Mobility API - dummy test plugin copy (so we have two)
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummyinvalid)

include(../../../../common.pri)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummyinvalid
DEFINES += DUMMYPLUGINNAME=invalid

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
