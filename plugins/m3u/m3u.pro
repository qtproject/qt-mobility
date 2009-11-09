TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(m3u)

PLUGIN_TYPE = playlistformats

include (../../common.pri)

qtAddLibrary(QtMedia)

DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia

HEADERS += qm3uhandler.h
SOURCES += main.cpp \
           qm3uhandler.cpp

target.path=$$QT_MOBILITY_PREFIX/plugins/playlistformats
INSTALLS+=target
