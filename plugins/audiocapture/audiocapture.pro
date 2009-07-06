TEMPLATE = lib
CONFIG += plugin
TARGET = audiocapture
DESTDIR = $$OUT_PWD/../../lib/mediaservice
DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

# Input
HEADERS += audiocapturecontrol.h \
    audiocaptureservice.h \
    audiocaptureserviceplugin.h

SOURCES += audiocapturecontrol.cpp \
    audiocaptureservice.cpp \
    audiocaptureserviceplugin.cpp
