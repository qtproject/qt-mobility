TARGET = tst_qmediaplayer
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

symbian {
    HEADERS += tst_qmediaplayer_s60.h
    SOURCES += tst_qmediaplayer_s60.cpp
}   
HEADERS += tst_qmediaplayer.h
SOURCES += main.cpp tst_qmediaplayer.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

