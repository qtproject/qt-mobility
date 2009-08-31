TEMPLATE = lib
CONFIG += plugin
TARGET = mpd
QT += network

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

LIBS += -lQtMedia

INCLUDEPATH += ../../multimedia

HEADERS = \
    mpddaemon.h \
    mpdplayercontrol.h \
    mpdplaylistcontrol.h \
    mpdplayerservice.h \
    mpdserviceplugin.h \
    mpdplaylistsource.h \
    mpdmetadata.h

SOURCES = \
    mpddaemon.cpp \
    mpdplayercontrol.cpp \
    mpdplaylistcontrol.cpp \
    mpdplayerservice.cpp \
    mpdserviceplugin.cpp \
    mpdplaylistsource.cpp \
    mpdmetadata.cpp
