TARGET = tst_qaudiocapturesource
INCLUDEPATH += ../../../multimedia

CONFIG += testcase

SOURCES += tst_qaudiocapturesource.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

