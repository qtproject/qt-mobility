TEMPLATE = lib
CONFIG += plugin
TARGET = qwmp

PLUGIN_SUBDIR = mediaservice

include(../../common.pri)

DEFINES += QWMP_EVR

LIBS += -lstrmiids -lole32 -lOleaut32 -luser32 -lgdi32

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

HEADERS = \
    qevrvideooverlay.h \
    qmfactivate.h \
    qwmpevents.h \
    qwmpglobal.h \
    qwmpmetadata.h \
    qwmpplayercontrol.h \
    qwmpplayerservice.h \
    qwmpplaylist.h \
    qwmpserviceprovider.h \
    qwmpvideooverlay.h

SOURCES = \
    qevrvideooverlay.cpp \
    qmfactivate.cpp \
    qwmpevents.cpp \
    qwmpglobal.cpp \
    qwmpmetadata.cpp \
    qwmpplayercontrol.cpp \
    qwmpplayerservice.cpp \
    qwmpplaylist.cpp \
    qwmpserviceprovider.cpp \
    qwmpvideooverlay.cpp

