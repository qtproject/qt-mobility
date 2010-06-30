TEMPLATE = app
TARGET = documentshare

include(../examples.pri)
CONFIG += mobility
MOBILITY = gallery

QT += network webkit

INCLUDEPATH += ../../src/gallery
DEPENDPATH += ../../src/gallery

HEADERS = \
    download.h \
    downloaddelegate.h \
    downloadmodel.h \
    iteminformationdialog.h \
    sharewidget.h

SOURCES = \
    download.cpp \
    downloaddelegate.cpp \
    downloadmodel.cpp \
    iteminformationdialog.cpp \
    main.cpp \
    sharewidget.cpp
