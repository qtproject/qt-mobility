TARGET = tst_qgalleryabstractrequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryabstractrequest.cpp

CONFIG += mobility
MOBILITY = gallery

