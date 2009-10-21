TARGET = tst_qmediaobject
CONFIG += testcase
INCLUDEPATH += ../../../multimedia

include (../../../common.pri)

SOURCES += tst_qmediaobject.cpp

qtAddLibrary(QtMedia)
