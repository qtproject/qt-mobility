TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmedia_blackberry)
PLUGIN_TYPE=mediaservice

include(../../../common.pri)
INCLUDEPATH+=../../../src/multimedia \
             ../../../src/multimedia/video \
             ../../../src/multimedia/audio

CONFIG += mobility
MOBILITY = multimedia

LIBS += -lmmrndclient -lstrm -lscreen

DEPENDPATH += .
HEADERS += \
    bbserviceplugin.h \
    bbmediaplayerservice.h \
    bbmediaplayercontrol.h \
    bbvideowindowcontrol.h \
    bbmetadata.h \
    bbutil.h

SOURCES += \
    bbserviceplugin.cpp \
    bbmediaplayerservice.cpp \
    bbmediaplayercontrol.cpp \
    bbvideowindowcontrol.cpp \
    bbmetadata.cpp \
    bbutil.cpp
