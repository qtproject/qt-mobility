TARGET = tst_qgalleryitemlist
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryitemlist.cpp

CONFIG += mobility
MOBILITY = gallery

