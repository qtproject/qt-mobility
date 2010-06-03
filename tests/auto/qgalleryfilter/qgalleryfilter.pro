TARGET = tst_qgalleryfilter
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryfilter.cpp

CONFIG += mobility
MOBILITY = gallery

