TARGET = tst_qradiotuner
INCLUDEPATH += ../../../src/multimedia

CONFIG += testcase

symbian {
    include(../../../config.pri)
    contains(openmaxal_symbian_enabled, yes) {
        DEFINES += HAS_OPENMAXAL_RADIOTUNER_BACKEND
        HEADERS += tst_qradiotuner_xa.h
        SOURCES += tst_qradiotuner_xa.cpp
        TARGET.CAPABILITY = ALL -TCB
    } else {
        HEADERS += tst_qradiotuner_s60.h
        SOURCES += tst_qradiotuner_s60.cpp
    }
}

HEADERS += tst_qradiotuner.h
SOURCES += main.cpp tst_qradiotuner.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

