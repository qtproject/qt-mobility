
TEMPLATE = lib
CONFIG += plugin
QT += phonon
TARGET = phononengine
include (../../common.pri)

LIBS += -lQtMedia
DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/lib/mediaservice
DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

HEADERS += \
    qphononplayercontrol.h \
    qphononplayerservice.h \
    qphononvideowidget.h \
    qphononserviceplugin.h \
    qphononmetadataprovider.h \

SOURCES += \
    qphononplayercontrol.cpp \
    qphononplayerservice.cpp \
    qphononvideowidget.cpp \
    qphononserviceplugin.cpp \
    qphononmetadataprovider.cpp \
