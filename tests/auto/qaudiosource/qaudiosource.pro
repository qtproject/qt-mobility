TARGET = tst_qaudiosource
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

SOURCES += tst_qaudiosource.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

