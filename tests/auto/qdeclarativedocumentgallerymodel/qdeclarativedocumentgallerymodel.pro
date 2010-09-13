TARGET = tst_qdeclarativedocumentgallerymodel
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qdeclarativedocumentgallerymodel.cpp

CONFIG += mobility
MOBILITY = gallery

QT += declarative
