TEMPLATE = lib
QT+=multimedia
CONFIG += plugin
QT+=multimedia
TARGET = $$qtLibraryTarget(dsengine)

PLUGIN_TYPE = mediaservice

include (../../../common.pri)
INCLUDEPATH+=../../../src/multimedia

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

HEADERS += dsserviceplugin.h
SOURCES += dsserviceplugin.cpp

TMP_INCLUDE = $$quote($$(INCLUDE))
TMP_SEARCHPATHS = $$split(TMP_INCLUDE, ";") $$QMAKE_INCDIR
TMP_REQUIRED_HEADERS =
for(p, TMP_SEARCHPATHS) {
    exists($${p}/qedit.h): TMP_REQUIRED_HEADERS *= qedit.h
}
contains(TMP_REQUIRED_HEADERS, qedit.h) {
    include(camera/camera.pri)
}
include (player/player.pri)
