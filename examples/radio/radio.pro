TEMPLATE = app
CONFIG += example

INCLUDEPATH += ../../src/multimedia
include(../examples.pri)

CONFIG += mobility
MOBILITY = multimedia

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    radio.h
  
SOURCES = \
    main.cpp \
    radio.cpp

