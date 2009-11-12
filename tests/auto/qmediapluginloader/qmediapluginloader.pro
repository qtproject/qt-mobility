TARGET = tst_qmediapluginloader
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

SOURCES += tst_qmediapluginloader.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

