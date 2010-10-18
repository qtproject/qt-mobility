TARGET = tst_qmediaobject
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

symbian {
    include(../../../config.pri)
    contains(openmaxal_symbian_enabled, yes) {
        DEFINES += HAS_OPENMAXAL_MEDIAPLAY_BACKEND
        HEADERS += tst_qmediaobject_xa.h
        SOURCES += tst_qmediaobject_xa.cpp
        TARGET.CAPABILITY = ALL -TCB

        testFiles.sources = testfiles/*
        testFiles.path = /Data/testfiles
        DEPLOYMENT += testFiles
    }else {
        HEADERS += tst_qmediaobject_mmf.h
        SOURCES += tst_qmediaobject_mmf.cpp
        TARGET.CAPABILITY = ALL -TCB

        testFiles.sources = testfiles/*
        testFiles.path = /Data/testfiles
        DEPLOYMENT += testFiles
    }
}

include (../../../common.pri)

HEADERS+= tst_qmediaobject.h
SOURCES += main.cpp tst_qmediaobject.cpp

CONFIG += mobility
MOBILITY = multimedia
