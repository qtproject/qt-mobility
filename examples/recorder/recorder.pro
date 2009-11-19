TEMPLATE = app
TARGET = recorder

INCLUDEPATH+=../../src/multimedia
include(../examples.pri)

qtAddLibrary(QtMedia)

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
