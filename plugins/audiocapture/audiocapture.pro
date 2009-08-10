TEMPLATE = lib
CONFIG += plugin
TARGET = audioengine

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

DEFINES += AUDIOSERVICES

DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

# Input
HEADERS += audioencodecontrol.h \
    audiomediacontrol.h \
    audiodeviceendpoint.h \
    audiocaptureservice.h \
    audiocaptureserviceplugin.h \
    audiocapturesession.h

SOURCES += audioencodecontrol.cpp \
    audiomediacontrol.cpp \
    audiodeviceendpoint.cpp \
    audiocaptureservice.cpp \
    audiocaptureserviceplugin.cpp \
    audiocapturesession.cpp
