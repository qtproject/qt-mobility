TARGET = tst_qaudiocapturesource
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio

CONFIG += testcase

SOURCES += tst_qaudiocapturesource.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

