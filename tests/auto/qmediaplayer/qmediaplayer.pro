TARGET = tst_qmediaplayer
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

symbian {
    include(../../../config.pri)
    contains(openmaxal_symbian_enabled, yes) {
        DEFINES += HAS_OPENMAXAL_MEDIAPLAY_BACKEND
        HEADERS += tst_qmediaplayer_xa.h
        SOURCES += tst_qmediaplayer_xa.cpp
        TARGET.CAPABILITY = ALL -TCB

        testFiles.sources = testfiles/*
        testFiles.path = /Data/testfiles
        DEPLOYMENT += testFiles
    } else {
        HEADERS += tst_qmediaplayer_s60.h
        SOURCES += tst_qmediaplayer_s60.cpp
        TARGET.CAPABILITY = ALL -TCB

        testFiles.sources = testfiles/*
        testFiles.path = /Data/testfiles
        DEPLOYMENT += testFiles
    }
}

HEADERS += tst_qmediaplayer.h
SOURCES += main.cpp tst_qmediaplayer.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

