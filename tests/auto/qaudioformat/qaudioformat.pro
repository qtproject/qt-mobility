TARGET = tst_qaudioformat
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio

include (../../../common.pri)


SOURCES += tst_qaudioformat.cpp

QT += core
CONFIG += mobility
MOBILITY = multimedia
