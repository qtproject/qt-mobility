TARGET = tst_qgalleryurlrequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryurlrequest.cpp

CONFIG += mobility
MOBILITY = gallery

