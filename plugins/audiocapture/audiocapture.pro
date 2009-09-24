TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(audioengine)

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

DEFINES += AUDIOSERVICES

DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia

# Input
HEADERS += audioencodercontrol.h \
    audiomediarecordercontrol.h \
    audiodevicecontrol.h \
    audiocaptureservice.h \
    audiocaptureserviceplugin.h \
    audiocapturesession.h

SOURCES += audioencodercontrol.cpp \
    audiomediarecordercontrol.cpp \
    audiodevicecontrol.cpp \
    audiocaptureservice.cpp \
    audiocaptureserviceplugin.cpp \
    audiocapturesession.cpp

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target
