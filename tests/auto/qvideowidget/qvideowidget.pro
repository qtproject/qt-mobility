TARGET = tst_qvideowidget
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qvideowidget.cpp

CONFIG += mobility
MOBILITY = multimedia

contains(QT_CONFIG,multimedia) {
    QT += multimedia
} else {
    INCLUDEPATH += ../../../src/multimedia/video
}

