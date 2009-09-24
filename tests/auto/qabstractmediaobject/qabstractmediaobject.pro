TARGET = tst_qabstractmediaobject
INCLUDEPATH+= ../../../multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qabstractmediaobject.cpp

qtAddLibrary(QtMedia)
