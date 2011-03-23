TEMPLATE = app
TARGET = documentproperties

include(../mobility_examples.pri)
CONFIG += mobility
MOBILITY = gallery

INCLUDEPATH += ../../src/gallery
DEPENDPATH += ../../src/gallery

HEADERS = \
        documentpropertieswidget.h \
        filebrowser.h \
        metadatabinding.h

SOURCES = \
        documentpropertieswidget.cpp \
        filebrowser.cpp \
        main.cpp \
        metadatabinding.cpp

symbian: {
    load(data_caging_paths)
    TARGET.CAPABILITY = ReadDeviceData WriteDeviceData
    }
