QT       += phonon
TARGET = playlist
TEMPLATE = app

INCLUDEPATH += ../../src/multimedia

qtAddLibrary(QtMedia)

LIBS +=  ../../playlistformats/m3u/libqm3u.a

SOURCES += main.cpp\
        imageviewer.cpp

HEADERS  += imageviewer.h

FORMS    += imageviewer.ui
