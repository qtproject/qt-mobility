TARGET = tst_qaudiocapturesource
INCLUDEPATH += ../../../src/multimedia

CONFIG += testcase

SOURCES += tst_qaudiocapturesource.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

contains(QT_CONFIG,multimedia) {
    QT += multimedia
} else {
    INCLUDEPATH += ../../../src/multimedia/audio
}
