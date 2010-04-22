TEMPLATE = app
TARGET = documentshare

include(../examples.pri)
CONFIG += mobility
MOBILITY = gallery

QT += network webkit

INCLUDEPATH += ../../src/gallery

HEADERS = \
    download.h \
    downloadcompletedialog.h \
    sharewidget.h

SOURCES = \
    download.cpp \
    downloadcompletedialog.cpp \
    main.cpp \
    sharewidget.cpp
