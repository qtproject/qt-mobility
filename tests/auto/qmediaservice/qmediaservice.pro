TARGET = tst_qmediaservice
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qmediaservice.cpp

qtAddLibrary(QtMedia)

