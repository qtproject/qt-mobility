TEMPLATE = app
TARGET = mediabrowser

include(../examples.pri)
CONFIG += mobility
MOBILITY = gallery

INCLUDEPATH += ../../src/gallery

HEADERS = \
    albumdelegate.h \
    albumview.h \
    artistdelegate.h \
    artistview.h \
    browser.h \
    gallerymodel.h \
    galleryview.h \
    songview.h

SOURCES = \
    albumdelegate.cpp \
    albumview.cpp \
    artistdelegate.cpp \
    artistview.cpp \
    browser.cpp \
    gallerymodel.cpp \
    galleryview.cpp \
    main.cpp \
    songview.cpp
