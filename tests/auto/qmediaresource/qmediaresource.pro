TARGET = tst_qmediaresource
CONFIG += testcase
INCLUDEPATH += ../../../multimedia

include (../../../common.pri)

SOURCES += tst_qmediaresource.cpp

qtAddLibrary(QtMedia)

