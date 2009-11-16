TEMPLATE = app
TARGET=streamplayer

INCLUDEPATH+=../../src/multimedia
include (../examples.pri)

qtAddLibrary(QtMedia)

HEADERS = \
    streamplayer.h

SOURCES = \
    main.cpp \
    streamplayer.cpp
