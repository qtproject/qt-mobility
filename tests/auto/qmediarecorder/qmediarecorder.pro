TARGET = tst_qmediarecorder
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio
CONFIG += testcase

SOURCES += tst_qmediarecorder.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

