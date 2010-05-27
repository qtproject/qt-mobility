TEMPLATE = lib
QT+=multimedia
CONFIG += plugin
QT+=multimedia
TARGET = $$qtLibraryTarget(qtmedia_dsengine)

PLUGIN_TYPE=mediaservice

include (../../../common.pri)
INCLUDEPATH+=../../../src/multimedia

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

HEADERS += dsserviceplugin.h
SOURCES += dsserviceplugin.cpp

!contains(TMP_REQUIRED_HEADERS, wmsdk.h): DEFINES += QT_NO_WMSDK

include (player/player.pri)
