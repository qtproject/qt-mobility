######################################################################
#
# Contacts Mobility API - dummy test plugin
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummy)

include(../../../../common.pri)

SOURCES += dummyplugin.cpp

DEFINES += DUMMYPLUGINNAME=testdummy

include(../../contacts_plugins.pri)
