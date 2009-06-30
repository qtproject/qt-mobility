
TEMPLATE = lib
CONFIG += plugin
TARGET = mpd
QT += network

INCLUDEPATH += ../../multimedia
LIBS += -L../../lib -lQtMedia

HEADERS = \
    mpddaemon.h \
    mpdplayercontrol.h \
    mpdplayerservice.h \
    mpdserviceplugin.h \
    mpdplaylistsource.h

SOURCES = \
    mpddaemon.cpp \
    mpdplayercontrol.cpp \
    mpdplayerservice.cpp \
    mpdserviceplugin.cpp \
    mpdplaylistsource.cpp
