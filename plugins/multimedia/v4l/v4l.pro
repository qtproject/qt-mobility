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

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
