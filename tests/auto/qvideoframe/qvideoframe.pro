TARGET = tst_qvideoframe
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video

include (../../../common.pri)

SOURCES += tst_qvideoframe.cpp

QT += core
CONFIG += mobility
MOBILITY = multimedia

