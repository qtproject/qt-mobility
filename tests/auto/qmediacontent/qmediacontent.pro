TARGET = tst_qmediacontent
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

qtAddLibrary(QtMedia)
SOURCES += tst_qmediacontent.cpp
