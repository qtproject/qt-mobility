######################################################################
#
# Contacts Mobility API - dummy test plugin with a different interface
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(contacts_testotherdummy)

# Stick it somewhere else so it doesn't get picked up by default
PLUGIN_SUBDIR=dummyplugin/plugins
include(../../../../common.pri)

SOURCES += dummyotherplugin.cpp
    
