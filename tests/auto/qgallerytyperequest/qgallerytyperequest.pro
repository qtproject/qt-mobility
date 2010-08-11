TARGET = tst_qgallerytyperequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgallerytyperequest.cpp

CONFIG += mobility
MOBILITY = gallery

