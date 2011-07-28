TEMPLATE = lib

CONFIG += plugin
TARGET = $$qtLibraryTarget(wmfengine)

PLUGIN_TYPE=mediaservice

include (../../../common.pri)
INCLUDEPATH+=../../../src/multimedia \
             ../../../src/multimedia/audio \
             ../../../src/multimedia/video

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

HEADERS += wmfserviceplugin.h
SOURCES += wmfserviceplugin.cpp

include (player/player.pri)
