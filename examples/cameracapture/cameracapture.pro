TEMPLATE = app
TARGET = cameracapture

INCLUDEPATH+=../../multimedia
include(../examples.pri)

qtAddLibrary(QtMedia)

HEADERS = cameracapture.h \
    settings.h
SOURCES = main.cpp \
    cameracapture.cpp \
    settings.cpp
FORMS += cameracapture.ui \
    settings.ui
