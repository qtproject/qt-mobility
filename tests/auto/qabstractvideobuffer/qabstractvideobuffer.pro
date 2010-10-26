TARGET = tst_qabstractvideobuffer
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video

include (../../../common.pri)

SOURCES += tst_qabstractvideobuffer.cpp

QT += core
CONFIG += mobility
MOBILITY = multimedia

