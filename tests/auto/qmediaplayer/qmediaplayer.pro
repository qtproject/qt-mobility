TARGET = tst_qmediaplayer
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

SOURCES += tst_qmediaplayer.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

