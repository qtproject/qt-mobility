TEMPLATE = app
TARGET = recorder

INCLUDEPATH+=../../multimedia
include(../examples.pri)

qtAddLibrary(QtMedia)

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
