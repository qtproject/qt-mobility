
TEMPLATE = lib
CONFIG += plugin
TARGET = qt7

INCLUDEPATH += ../../multimedia ../../multimedia/endpoints
LIBS += -L../../lib -lQtMedia -framework QTKit -framework QuickTime
DESTDIR = ../../lib/mediaservice

HEADERS = \
    qt7playercontrol.h \
    qt7playerservice.h \
    qt7serviceplugin.h \
    qt7metadata.h \
    qt7widget.h \
    qt7movie.h

SOURCES = \
    qt7playercontrol.cpp \
    qt7playerservice.cpp \
    qt7serviceplugin.cpp \
    qt7metadata.cpp \
    qt7widget.mm \
    qt7movie.mm
