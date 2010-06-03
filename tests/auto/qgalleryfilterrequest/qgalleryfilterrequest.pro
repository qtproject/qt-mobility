TARGET = tst_qgalleryfilterrequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryfilterrequest.cpp

CONFIG += mobility
MOBILITY = gallery

