TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(landmarks_testdummydynamic)
PLUGIN_TYPE=landmarks

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/location \
               ../../../../src/location/landmarks

CONFIG += mobility
MOBILITY = location

DEFINES += DUMMYPLUGINNAME=LandmarkManagerFactoryDummyDynamic
DEFINES += DUMMYPLUGINTARGET=landmarks_testdummydynamic

HEADERS += dummydynamicplugin.h

include(../../landmarks_plugins.pri)
