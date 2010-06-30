TARGET = tst_qgallerybaseresponse
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgallerybaseresponse.cpp

CONFIG += mobility
MOBILITY = gallery

