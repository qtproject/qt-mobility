#-------------------------------------------------
#
# Project created by QtCreator 2009-06-23T14:24:15
#
#-------------------------------------------------

QT       += phonon

TARGET = playlist
TEMPLATE = app

INCLUDEPATH += ../..

LIBS +=  ../../playlistformats/m3u/libqm3u.a ../../libqmultimedia.a

SOURCES += main.cpp\
        imageviewer.cpp

HEADERS  += imageviewer.h

FORMS    += imageviewer.ui
