TARGET = tst_qgraphicsvideoitem
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

SOURCES += tst_qgraphicsvideoitem.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

