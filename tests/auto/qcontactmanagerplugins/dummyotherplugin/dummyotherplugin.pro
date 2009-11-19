TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testotherdummy)
PLUGIN_TYPE=contacts

include(../../../../common.pri)

SOURCES += dummyotherplugin.cpp
    
include(../../contacts_plugins.pri)
