TARGET = tst_qradiotuner
INCLUDEPATH += ../../../multimedia

CONFIG += testcase

SOURCES += tst_qradiotuner.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

