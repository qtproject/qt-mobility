TARGET = tst_qmediaplayer
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

SOURCES += tst_qmediaplayer.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

