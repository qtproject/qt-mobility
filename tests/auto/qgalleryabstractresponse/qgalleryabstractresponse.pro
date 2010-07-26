TARGET = tst_qgalleryabstractresponse
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryabstractresponse.cpp

CONFIG += mobility
MOBILITY = gallery

