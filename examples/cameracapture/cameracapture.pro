TEMPLATE = app
TARGET = cameracapture

INCLUDEPATH+=../../multimedia
include(../examples.pri)

qtAddLibrary(QtMedia)

HEADERS = cameracapture.h

SOURCES = \
    main.cpp \
    cameracapture.cpp

FORMS += cameracapture.ui
