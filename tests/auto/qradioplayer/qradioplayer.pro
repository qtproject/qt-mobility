TARGET = tst_qradioplayer
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

SOURCES += tst_qradioplayer.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

