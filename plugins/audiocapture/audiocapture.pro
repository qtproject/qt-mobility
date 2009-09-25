TEMPLATE = lib
CONFIG += plugin
TARGET = audioengine

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

LIBS += -lQtMedia

DEPENDPATH += .

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
