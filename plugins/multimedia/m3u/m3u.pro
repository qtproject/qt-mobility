TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(m3u)

PLUGIN_TYPE = playlistformats

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .
INCLUDEPATH += . \
    ../../../src/multimedia

HEADERS += qm3uhandler.h
SOURCES += main.cpp \
           qm3uhandler.cpp
symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}

target.path=$$QT_MOBILITY_PREFIX/plugins/playlistformats
INSTALLS+=target
