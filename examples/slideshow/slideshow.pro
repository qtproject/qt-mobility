TEMPLATE = app
TARGET = slideshow
INCLUDEPATH+=../../src/multimedia
include (../examples.pri)

qtAddLibrary(QtMedia)

HEADERS = \
    slideshow.h

SOURCES = \
    main.cpp \
    slideshow.cpp

