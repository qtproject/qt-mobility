TARGET = tst_qgraphicsvideoitem
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

SOURCES += tst_qgraphicsvideoitem.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

