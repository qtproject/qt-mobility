TARGET = tst_qgraphicsvideoitem
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

QT += multimedia

SOURCES += tst_qgraphicsvideoitem.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

