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

include(camera/camera.pri)

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target


