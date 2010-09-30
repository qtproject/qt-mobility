TARGET = tst_qdeclarativedocumentgalleryitem
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/gallery

SOURCES += tst_qdeclarativedocumentgalleryitem.cpp

CONFIG += mobility
MOBILITY = gallery

QT += declarative
