TARGET = tst_declarativedocumentgallerytype
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qdeclarativedocumentgallerytype.cpp

CONFIG += mobility
MOBILITY = gallery

QT += declarative
