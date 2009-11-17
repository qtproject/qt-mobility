TARGET = tst_qradiotuner
INCLUDEPATH += ../../../src/multimedia

CONFIG += testcase

SOURCES += tst_qradiotuner.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

