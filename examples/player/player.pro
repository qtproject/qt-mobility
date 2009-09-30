TEMPLATE = app
CONFIG += example
include (../../common.pri)
LIBS += -lQtMedia
HEADERS = player.h \
    playercontrols.h \
    playlistmodel.h \
    videowidget.h
SOURCES = main.cpp \
    player.cpp \
    playercontrols.cpp \
    playlistmodel.cpp \
    videowidget.cpp
