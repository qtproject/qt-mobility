TARGET = tst_qmediaresource
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia

include (../../../common.pri)

SOURCES += tst_qmediaresource.cpp

qtAddLibrary(QtMedia)

