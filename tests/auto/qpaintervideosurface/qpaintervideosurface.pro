TARGET = tst_qpaintervideosurface
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video

contains(QT_CONFIG, opengl): QT += opengl

include (../../../common.pri)

SOURCES += tst_qpaintervideosurface.cpp

CONFIG += mobility
MOBILITY = multimedia

