TARGET = tst_qgalleryitemlistmodel
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryitemlistmodel.cpp

CONFIG += mobility
MOBILITY = gallery

