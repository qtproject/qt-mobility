TARGET = tst_qabstractmediaservice
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qabstractmediaservice.cpp

qtAddLibrary(QtMedia)

