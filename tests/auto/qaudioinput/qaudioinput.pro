TARGET = tst_qaudioinput
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
SOURCES += wavheader.cpp tst_qaudioinput.cpp

QT += core
CONFIG += mobility
MOBILITY = multimedia

