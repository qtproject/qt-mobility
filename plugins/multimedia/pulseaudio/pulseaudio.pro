TEMPLATE = lib
CONFIG += plugin
QT += network
TARGET = $$qtLibraryTarget(pulseaudio)
PLUGIN_TYPE = mediaservice

include (../../../common.pri)

CONFIG += mobility link_pkgconfig
MOBILITY = multimedia

maemo5:DEFINES+=MAEMO_VOLUME

DEPENDPATH += .
INCLUDEPATH += ../../../src/multimedia
PKGCONFIG += libpulse

HEADERS += wavedecoder.h \
    pulseaudioserviceproviderplugin.h \
    pulseaudioplayerservice.h \
    pulseaudioplayercontrol.h

SOURCES += wavedecoder.cpp \
    pulseaudioserviceproviderplugin.cpp \
    pulseaudioplayerservice.cpp \
    pulseaudioplayercontrol.cpp

target.path = $$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS += target
