TEMPLATE = app
CONFIG += example
include(../../common.pri)
LIBS += -lQtMedia
HEADERS = cameracapture.h \
    settings.h
SOURCES = main.cpp \
    cameracapture.cpp \
    settings.cpp
FORMS += cameracapture.ui \
    settings.ui
