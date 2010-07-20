TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmedia_v4lengine)

PLUGIN_TYPE = mediaservice

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

INCLUDEPATH += ../../../src/multimedia

HEADERS += v4lserviceplugin.h
SOURCES += v4lserviceplugin.cpp

include(radio/radio.pri)
