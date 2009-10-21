TEMPLATE = app
TARGET = player

INCLUDEPATH += ../../multimedia

include(../examples.pri)
qtAddLibrary(QtMedia)

HEADERS = \
    player.h \
    playercontrols.h \
    playlistmodel.h \
    videowidget.h
SOURCES = main.cpp \
    player.cpp \
    playercontrols.cpp \
    playlistmodel.cpp \
    videowidget.cpp
