
TEMPLATE = lib
CONFIG += plugin
TARGET = mpd
QT += network

include (../../common.pri)
INCLUDEPATH += ../../multimedia ../../multimedia/endpoints
LIBS += -lQtMedia
DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/lib/mediaservice

HEADERS = \
    mpddaemon.h \
    mpdplayercontrol.h \
    mpdplayerservice.h \
    mpdserviceplugin.h \
    mpdplaylistsource.h \
    mpdmetadata.h

SOURCES = \
    mpddaemon.cpp \
    mpdplayercontrol.cpp \
    mpdplayerservice.cpp \
    mpdserviceplugin.cpp \
    mpdplaylistsource.cpp \
    mpdmetadata.cpp
