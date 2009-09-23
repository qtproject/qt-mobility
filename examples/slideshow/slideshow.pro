TEMPLATE = app
TARGET = slideshow
CONFIG += example

include (../../common.pri)

LIBS += -lQtMedia

HEADERS = \
    slideshow.h

SOURCES = \
    main.cpp \
    slideshow.cpp

