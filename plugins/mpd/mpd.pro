
TEMPLATE = lib
CONFIG += plugin
TARGET = mpd
QT += network

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

LIBS += -lQtMedia

INCLUDEPATH += ../../multimedia ../../multimedia/endpoints

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
