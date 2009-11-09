TARGET = tst_qmediarecorder
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

SOURCES += tst_qmediarecorder.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

