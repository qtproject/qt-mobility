TARGET = tst_declarativedocumentgalleryitem
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qdeclarativedocumentgalleryitem.cpp

CONFIG += mobility
MOBILITY = gallery

QT += declarative
