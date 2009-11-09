TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(dsengine)

PLUGIN_TYPE = mediaservice

include (../../common.pri)
INCLUDEPATH+=../../multimedia

qtAddLibrary(QtMedia)

DEPENDPATH += .

HEADERS += dsserviceplugin.h
SOURCES += dsserviceplugin.cpp

include(camera/camera.pri)
