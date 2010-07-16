TARGET = tst_qdocumentgallery
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qdocumentgallery.cpp

CONFIG += mobility
MOBILITY = gallery

