TARGET = tst_qpaintervideosurface
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia

QT += multimedia

contains(QT_CONFIG, opengl): QT += opengl

include (../../../common.pri)

SOURCES += tst_qpaintervideosurface.cpp

CONFIG += mobility
MOBILITY = multimedia
