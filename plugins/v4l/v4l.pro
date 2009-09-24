TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(v4lengine)

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

qtAddLibrary(QtMedia)

DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

HEADERS += v4lserviceplugin.h
SOURCES += v4lserviceplugin.cpp

include(radio/radio.pri)
#include(camera/camera.pri)

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target
