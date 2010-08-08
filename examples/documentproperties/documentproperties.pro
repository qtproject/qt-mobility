TEMPLATE = app
TARGET = documentproperties

include(../examples.pri)
CONFIG += mobility
MOBILITY = gallery

INCLUDEPATH += ../../src/gallery
DEPENDPATH += ../../src/gallery

HEADERS = \
        documentpropertieswidget.h \
        filebrowser.h

SOURCES = \
        documentpropertieswidget.cpp \
        filebrowser.cpp \
        main.cpp
