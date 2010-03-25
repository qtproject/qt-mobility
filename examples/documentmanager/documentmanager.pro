TEMPLATE = app
TARGET = documentmanager

include(../examples.pri)
CONFIG += mobility
MOBILITY = gallery

INCLUDEPATH += ../../src/gallery

HEADERS = \
    gallerymodel.h \
    manager.h

SOURCES = \
    gallerymodel.cpp \
    manager.cpp \
    main.cpp
