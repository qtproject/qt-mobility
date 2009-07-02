
TEMPLATE = lib
CONFIG += plugin
TARGET = mpd
QT += network

INCLUDEPATH += ../../multimedia ../../multimedia/endpoints
LIBS += -L../../lib -lQtMedia

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
