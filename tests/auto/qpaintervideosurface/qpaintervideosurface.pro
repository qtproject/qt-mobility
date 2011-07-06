TARGET = tst_qpaintervideosurface
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video

contains(QT_CONFIG, opengl) | contains(QT_CONFIG, opengles2): !symbian {
   QT += opengl
} else {
   DEFINES += QT_NO_OPENGL
}

include (../../../common.pri)

SOURCES += tst_qpaintervideosurface.cpp

CONFIG += mobility
MOBILITY = multimedia

