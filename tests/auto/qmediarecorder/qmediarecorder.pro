TARGET = tst_qmediarecorder
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio
CONFIG += testcase

symbian {
    include(../../../config.pri)
    HEADERS += s60common.h
    contains(openmaxal_symbian_enabled, yes) {
    	DEFINES += HAS_OPENMAXAL_MEDIARECORD_BACKEND
        HEADERS += tst_qmediarecorder_xa.h
	SOURCES += tst_qmediarecorder_xa.cpp
    } else {
        HEADERS += tst_qmediarecorder_mmf.h
        SOURCES += tst_qmediarecorder_mmf.cpp
    }
	    
}
HEADERS += tst_qmediarecorder.h
SOURCES += main.cpp tst_qmediarecorder.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

