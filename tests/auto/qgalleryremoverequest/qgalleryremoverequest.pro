TARGET = tst_qgalleryremoverequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryremoverequest.cpp

CONFIG += mobility
MOBILITY = gallery

