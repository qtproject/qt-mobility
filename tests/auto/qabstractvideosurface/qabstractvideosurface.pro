TARGET = tst_qabstractvideosurface
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video

include (../../../common.pri)

SOURCES += tst_qabstractvideosurface.cpp

QT += core
CONFIG += mobility
MOBILITY = multimedia

