TEMPLATE = lib
CONFIG += plugin

QT+=multimedia
DEFINES += VIDEOSERVICES

TARGET = videoinput
DESTDIR = $$OUT_PWD/../../lib/mediaservice
DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

# Input
HEADERS += cameracontrol.h \
    cameraservice.h \
    cameraserviceplugin.h

SOURCES += cameracontrol.cpp \
    cameraservice.cpp \
    cameraserviceplugin.cpp
