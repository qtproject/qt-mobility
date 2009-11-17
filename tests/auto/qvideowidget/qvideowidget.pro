TARGET = tst_qvideowidget
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qvideowidget.cpp

qtAddLibrary(QtMedia)
