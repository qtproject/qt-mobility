######################################################################
#
# Contacts Mobility API - dummy test plugin with a different interface
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = contacts_testotherdummy

include(../../../../common.pri)

SOURCES += dummyotherplugin.cpp
    
include(../../contacts_plugins.pri)
