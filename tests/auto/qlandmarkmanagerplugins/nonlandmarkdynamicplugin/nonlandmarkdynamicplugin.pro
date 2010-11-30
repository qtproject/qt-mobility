TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(nonlandmarkdynamicplugin)
PLUGIN_TYPE=landmarks

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/location \
               ../../../../src/location/landmarks

CONFIG += mobility
MOBILITY = location

HEADERS += nonlandmarkdynamicplugin.h

include(../../landmarks_plugins.pri)
