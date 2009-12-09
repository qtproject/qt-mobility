TEMPLATE = app
TARGET = recorder

INCLUDEPATH+=../../src/multimedia
include(../examples.pri)

CONFIG += mobility
MOBILITY = multimedia

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
