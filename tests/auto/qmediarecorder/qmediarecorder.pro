TARGET = tst_qmediarecorder
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

SOURCES += tst_qmediarecorder.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

contains(QT_CONFIG,multimedia) {
    QT += multimedia
}
