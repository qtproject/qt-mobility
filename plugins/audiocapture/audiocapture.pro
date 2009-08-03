TEMPLATE = lib
CONFIG += plugin

#QT += multimedia
#DEFINES += AUDIOSERVICES

TARGET = audiocapture
DESTDIR = $$OUT_PWD/../../lib/mediaservice
DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

# Input
HEADERS += audioencode.h \
    audiodeviceendpoint.h \
    audiocaptureservice.h \
    audiocaptureserviceplugin.h \
    audiocapturesession.h

SOURCES += audioencode.cpp \
    audiodeviceendpoint.cpp \
    audiocaptureservice.cpp \
    audiocaptureserviceplugin.cpp \
    audiocapturesession.cpp
