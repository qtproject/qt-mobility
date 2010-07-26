TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(nonlandmarkdynamicplugin)
PLUGIN_TYPE=landmarks

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/location

CONFIG += mobility
MOBILITY = location

HEADERS += nonlandmarkdynamicplugin.h
