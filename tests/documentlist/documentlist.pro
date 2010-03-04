TEMPLATE = app

include(../../common.pri)

INCLUDEPATH += $$SOURCE_DIR/src/gallery
CONFIG += mobility
MOBILITY = gallery

include(../../examples/examples.pri)

HEADERS = \
        documentlistmodel.h \
        viewer.h

SOURCES = \
        documentlistmodel.cpp \
        main.cpp \
        viewer.cpp
