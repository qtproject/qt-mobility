TARGET = tst_qmediaplaylist
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

SOURCES += tst_qmediaplaylist.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

