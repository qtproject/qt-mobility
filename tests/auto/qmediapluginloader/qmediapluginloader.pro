TARGET = tst_qmediapluginloader
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

SOURCES += tst_qmediapluginloader.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

