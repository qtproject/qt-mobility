TEMPLATE = app
TARGET = slideshow
INCLUDEPATH+=../../src/multimedia
include (../examples.pri)

CONFIG += mobility
MOBILITY = multimedia

HEADERS = \
    slideshow.h

SOURCES = \
    main.cpp \
    slideshow.cpp

