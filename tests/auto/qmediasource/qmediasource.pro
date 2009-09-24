TARGET = tst_qmediasource
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qmediasource.cpp

qtAddLibrary(QtMedia)

