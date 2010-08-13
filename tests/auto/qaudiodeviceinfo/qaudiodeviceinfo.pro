TARGET = tst_qaudiodeviceinfo
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio

include (../../../common.pri)


SOURCES += tst_qaudiodeviceinfo.cpp

QT += core
CONFIG += mobility
MOBILITY = multimedia

