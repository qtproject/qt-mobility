TARGET = tst_qmediaobject
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia

include (../../../common.pri)

SOURCES += tst_qmediaobject.cpp

qtAddLibrary(QtMedia)
