TEMPLATE = lib
CONFIG += plugin
QT+=multimedia
TARGET = $$qtLibraryTarget(dsengine)

PLUGIN_TYPE = mediaservice

include (../../../common.pri)
INCLUDEPATH+=../../../src/multimedia

qtAddLibrary(QtMedia)

DEPENDPATH += .

HEADERS += dsserviceplugin.h
SOURCES += dsserviceplugin.cpp

include(camera/camera.pri)
