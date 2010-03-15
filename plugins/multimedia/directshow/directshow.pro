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
    exists($${p}/wmsdk.h): TMP_REQUIRED_HEADERS *= wmsdk.h
}
contains(TMP_REQUIRED_HEADERS, qedit.h) {
    include(camera/camera.pri)
}
!contains(TMP_REQUIRED_HEADERS, wmsdk.h): DEFINES += QT_NO_WMSDK

include (player/player.pri)

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target

