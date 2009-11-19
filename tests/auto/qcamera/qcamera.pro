TARGET = tst_qcamera
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

SOURCES += tst_qcamera.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

