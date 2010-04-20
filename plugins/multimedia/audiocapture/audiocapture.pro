TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmedia_audioengine)
PLUGIN_TYPE=mediaservice

QT += multimedia

include (../../../common.pri)
INCLUDEPATH += ../../../src/multimedia

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

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}
target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target
