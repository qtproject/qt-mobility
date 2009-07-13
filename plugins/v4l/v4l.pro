TEMPLATE = lib
CONFIG += plugin

TARGET = radio
DESTDIR = $$OUT_PWD/../../lib/mediaservice
DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

# Input
HEADERS += radiocontrol.h \
    radioservice.h \
    radioserviceplugin.h

SOURCES += radiocontrol.cpp \
    radioservice.cpp \
    radioserviceplugin.cpp
