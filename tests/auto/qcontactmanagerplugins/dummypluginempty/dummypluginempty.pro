######################################################################
#
# Contacts Mobility API - dummy test plugin copy (so we have two)
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummyempty)

include(../../../../common.pri)

DEFINES += DUMMYPLUGINTARGET=contacts_testdummyempty

SOURCES += ../dummyplugin/dummyplugin.cpp

include(../../contacts_plugins.pri)
