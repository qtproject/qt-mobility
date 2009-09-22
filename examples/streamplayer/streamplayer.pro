TEMPLATE = app
CONFIG += example

include (../../common.pri)

LIBS += -lQtMedia

HEADERS = \
    streamplayer.h

SOURCES = \
    main.cpp \
    streamplayer.cpp
