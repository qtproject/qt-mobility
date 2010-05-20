TARGET = tst_qaudiooutput
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio

include (../../../common.pri)

wince*: {
    DEFINES += SRCDIR=\\\"\\\"
    QT += gui
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

HEADERS += wavheader.h
SOURCES += wavheader.cpp tst_qaudiooutput.cpp

QT += core
CONFIG += mobility
MOBILITY = multimedia

