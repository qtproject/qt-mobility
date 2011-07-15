TARGET = tst_qradiotuner
INCLUDEPATH += ../../../src/multimedia

CONFIG += testcase

symbian {
    include(../../../config.pri)
        HEADERS += tst_qradiotuner_xa.h
        SOURCES += tst_qradiotuner_xa.cpp
        TARGET.CAPABILITY = ALL -TCB
        HEADERS += tst_qradiotuner_s60.h
        SOURCES += tst_qradiotuner_s60.cpp
    
}

HEADERS += tst_qradiotuner.h
SOURCES += main.cpp tst_qradiotuner.cpp

include (../../../common.pri)

include (../qmultimedia_common/mock.pri)
include (../qmultimedia_common/mockradio.pri)

CONFIG += mobility
MOBILITY = multimedia

