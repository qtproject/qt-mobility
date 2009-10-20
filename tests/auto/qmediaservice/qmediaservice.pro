TARGET = tst_qmediaservice
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qmediaservice.cpp

qtAddLibrary(QtMedia)

