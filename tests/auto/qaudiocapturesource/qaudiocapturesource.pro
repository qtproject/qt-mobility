TARGET = tst_qaudiocapturesource
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio

CONFIG += testcase

SOURCES += tst_qaudiocapturesource.cpp

include (../../../common.pri)
include (../qmultimedia_common/mockrecorder.pri)
include (../qmultimedia_common/mock.pri)

CONFIG += mobility
MOBILITY = multimedia

