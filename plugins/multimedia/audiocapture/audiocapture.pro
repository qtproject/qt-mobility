TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmedia_audioengine)
PLUGIN_TYPE=mediaservice

include (../../../common.pri)
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

# Input
HEADERS += audioencodercontrol.h \
    audiocontainercontrol.h \
    audiomediarecordercontrol.h \
    audioendpointselector.h \
    audiocaptureservice.h \
    audiocaptureserviceplugin.h \
    audiocapturesession.h

SOURCES += audioencodercontrol.cpp \
    audiocontainercontrol.cpp \
    audiomediarecordercontrol.cpp \
    audioendpointselector.cpp \
    audiocaptureservice.cpp \
    audiocaptureserviceplugin.cpp \
    audiocapturesession.cpp
