TEMPLATE = app
CONFIG += example

include (../../common.pri)

INCLUDEPATH += $$PWD/../../multimedia

LIBS += -lQtMedia

HEADERS = \
    streamplayer.h

SOURCES = \
    main.cpp \
    streamplayer.cpp
