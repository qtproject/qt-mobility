TARGET = tst_qvideowidget
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qvideowidget.cpp

qtAddLibrary(QtMedia)
