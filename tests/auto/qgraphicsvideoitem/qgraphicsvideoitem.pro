TARGET = tst_qgraphicsvideoitem
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

SOURCES += tst_qgraphicsvideoitem.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

contains(QT_CONFIG,multimedia) {
    QT += multimedia
}
