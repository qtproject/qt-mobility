TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmedia_wmp)

PLUGIN_TYPE=mediaservice

INCLUDEPATH+=../../../src/multimedia
include(../../../common.pri)

TMP_INCLUDE = $$quote($$(INCLUDE))
TMP_SEARCHPATHS = $$split(TMP_INCLUDE, ";") $$QMAKE_INCDIR
for(p, TMP_SEARCHPATHS): exists($${p}/evr.h): DEFINES *= QWMP_EVR

CONFIG += mobility
MOBILITY = multimedia
LIBS += -lstrmiids -lole32 -lOleaut32 -luser32 -lgdi32

HEADERS = \
    qevrvideooverlay.h \
    qmfactivate.h \
    qwmpevents.h \
    qwmpglobal.h \
    qwmpmetadata.h \
    qwmpplayercontrol.h \
    qwmpplayerservice.h \
    qwmpplaylist.h \
    qwmpplaylistcontrol.h \
    qwmpserviceprovider.h \
    qwmpvideooutputcontrol.h \
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
    qwmpplaylistcontrol.cpp \
    qwmpserviceprovider.cpp \
    qwmpvideooutputcontrol.cpp \
    qwmpvideooverlay.cpp

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
