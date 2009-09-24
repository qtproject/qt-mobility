TEMPLATE = app
TARGET=streamplayer

INCLUDEPATH+=../../multimedia
include (../examples.pri)

qtAddLibrary(QtMedia)

HEADERS = \
    streamplayer.h

SOURCES = \
    main.cpp \
    streamplayer.cpp
