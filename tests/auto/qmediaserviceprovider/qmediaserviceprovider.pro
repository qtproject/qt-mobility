TARGET = tst_qmediaserviceprovider
INCLUDEPATH += ../../../src/multimedia ../../../src
CONFIG += testcase

SOURCES += tst_qmediaserviceprovider.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

contains(QT_CONFIG,multimedia) {
    QT += multimedia
}
