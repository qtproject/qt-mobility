TARGET = tst_qmediaobject
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia

symbian {
    include(../../../config.pri)
    contains(openmaxal_symbian_enabled, yes) {
        DEFINES += HAS_OPENMAXAL_MEDIAPLAY_BACKEND
        HEADERS += tst_qmediaobject_xa.h
        SOURCES += tst_qmediaobject_xa.cpp

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
