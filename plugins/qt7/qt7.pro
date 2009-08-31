
TEMPLATE = lib
CONFIG += plugin
TARGET = qt7

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

INCLUDEPATH += ../../multimedia
LIBS += -lQtMedia -framework QTKit -framework QuickTime


HEADERS = \
    qt7playercontrol.h \
    qt7playerservice.h \
    qt7serviceplugin.h \
    qt7metadata.h \
    qt7movie.h

SOURCES = \
    qt7playercontrol.cpp \
    qt7playerservice.cpp \
    qt7serviceplugin.cpp \
    qt7metadata.cpp \
    qt7movie.mm
