TARGET = tst_qvideosurfaceformat
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video

include (../../../common.pri)

SOURCES += tst_qvideosurfaceformat.cpp

QT += core
CONFIG += mobility
MOBILITY = multimedia

contains(QT_CONFIG,multimedia) {
    QT += multimedia
}

