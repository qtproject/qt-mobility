TEMPLATE = app
TARGET=streamplayer

INCLUDEPATH+=../../src/multimedia
include (../examples.pri)

CONFIG += mobility
MOBILITY = multimedia

HEADERS = \
    streamplayer.h

SOURCES = \
    main.cpp \
    streamplayer.cpp
