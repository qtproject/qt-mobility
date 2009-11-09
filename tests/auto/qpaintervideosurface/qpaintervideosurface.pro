TARGET = tst_qpaintervideosurface
CONFIG += testcase
INCLUDEPATH += ../../../multimedia

contains(QT_CONFIG, opengl): QT += opengl

include (../../../common.pri)

SOURCES += tst_qpaintervideosurface.cpp

qtAddLibrary(QtMedia)
