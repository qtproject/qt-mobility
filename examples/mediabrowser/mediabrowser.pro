TEMPLATE = app
TARGET = mediabrowser

include(../examples.pri)
CONFIG += mobility
MOBILITY = gallery

INCLUDEPATH += ../../src/gallery
DEPENDPATH += ../../src/gallery

HEADERS = \
    albumdelegate.h \
    albumview.h \
    artistdelegate.h \
    artistview.h \
    browser.h \
    coverartmodel.h \
    galleryview.h \
    photodelegate.h \
    photoview.h \
    songview.h \
    thumbnailcache.h \
    thumbnailmodel.h

SOURCES = \
    albumdelegate.cpp \
    albumview.cpp \
    artistdelegate.cpp \
    artistview.cpp \
    browser.cpp \
    coverartmodel.cpp \
    galleryview.cpp \
    main.cpp \
    photodelegate.cpp \
    photoview.cpp \
    songview.cpp \
    thumbnailcache.cpp \
    thumbnailmodel.cpp

symbian: {
    load(data_caging_paths)
    TARGET.CAPABILITY = ReadDeviceData WriteDeviceData
}
