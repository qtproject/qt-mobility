TEMPLATE = app
TARGET = mediabrowser

include(../examples.pri)
CONFIG += mobility
MOBILITY = gallery

INCLUDEPATH += ../../src/gallery

HEADERS = \
    albumview.h \
    artistview.h \
    browser.h \
    gallerymodel.h \
    galleryview.h \
    songview.h

SOURCES = \
    albumview.cpp \
    artistview.cpp \
    browser.cpp \
    gallerymodel.cpp \
    galleryview.cpp \
    main.cpp \
    songview.cpp
