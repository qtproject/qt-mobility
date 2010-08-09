TARGET = tst_qgalleryitemrequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryitemrequest.cpp

CONFIG += mobility
MOBILITY = gallery

