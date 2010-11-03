TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testotherdummy)
PLUGIN_TYPE=contacts

INCLUDEPATH += $$MOC_DIR

include(../../../../common.pri)

SOURCES += dummyotherplugin.cpp
    
include(../../contacts_plugins.pri)
