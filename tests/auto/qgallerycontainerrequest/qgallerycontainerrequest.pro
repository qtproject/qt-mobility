TARGET = tst_qgallerycontainerrequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgallerycontainerrequest.cpp

CONFIG += mobility
MOBILITY = gallery

