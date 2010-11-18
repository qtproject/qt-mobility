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

symbian {
    TARGET.CAPABILITY = NetworkServices
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
