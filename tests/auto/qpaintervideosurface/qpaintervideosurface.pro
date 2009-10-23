TARGET = tst_qpaintervideosurface
CONFIG += testcase

contains(QT_CONFIG, opengl): QT += opengl

include (../../../common.pri)

SOURCES += tst_qpaintervideosurface.cpp

LIBS += -lQtMedia
