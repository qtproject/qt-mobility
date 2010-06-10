TARGET = tst_qgalleryqueryrequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryqueryrequest.cpp

CONFIG += mobility
MOBILITY = gallery

