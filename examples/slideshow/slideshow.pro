TEMPLATE = app
TARGET = slideshow
CONFIG += example

include (../../common.pri)

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

LIBS += -lQtMedia

HEADERS = \
    slideshow.h

SOURCES = \
    main.cpp \
    slideshow.cpp

