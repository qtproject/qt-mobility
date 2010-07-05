TARGET = tst_qgalleryresource
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryresource.cpp

CONFIG += mobility
MOBILITY = gallery

