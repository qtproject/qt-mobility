TEMPLATE = lib
CONFIG += plugin
TARGET = qwmp

DEFINES += QWMP_EVR

DESTDIR = $$OUT_PWD/../../lib
DLLDESTDIR = $$OUT_PWD/../../bin/mediaservice

LIBS += -L$$OUT_PWD/../../lib -lQtMedia -lstrmiids -lole32 -lOleaut32 -luser32 -lgdi32

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

