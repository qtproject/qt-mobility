TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(landmarks_testdummydynamiccopy)
PLUGIN_TYPE=landmarks

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/location

CONFIG += mobility
MOBILITY = location

DEFINES += DUMMYPLUGINNAME=LandmarkManagerFactoryDummyDynamic
DEFINES += DUMMYPLUGINTARGET=landmarks_testdummydynamiccopy

HEADERS += ../dummydynamicplugin/dummydynamicplugin.h
