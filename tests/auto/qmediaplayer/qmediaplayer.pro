TARGET = tst_qmediaplayer
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

symbian {
    include(../../../config.pri)
    TARGET.CAPABILITY = NetworkServices
    testFiles.sources = testfiles/*
    testFiles.path = /Data/testfiles
    DEPLOYMENT += testFiles
    HEADERS += tst_qmediaplayer_xa.h
    SOURCES += tst_qmediaplayer_xa.cpp
    HEADERS += tst_qmediaplayer_s60.h
    SOURCES += tst_qmediaplayer_s60.cpp

}

HEADERS += tst_qmediaplayer.h
SOURCES += main.cpp tst_qmediaplayer.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

