TEMPLATE = lib

CONFIG += plugin
TARGET = $$qtLibraryTarget(dsengine)

PLUGIN_TYPE=mediaservice

include (../../../common.pri)
INCLUDEPATH+=../../../src/multimedia \
             ../../../src/multimedia/audio \
             ../../../src/multimedia/video

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

HEADERS += dsserviceplugin.h
SOURCES += dsserviceplugin.cpp

!contains(TMP_REQUIRED_HEADERS, wmsdk.h): DEFINES += QT_NO_WMSDK

include (player/player.pri)
