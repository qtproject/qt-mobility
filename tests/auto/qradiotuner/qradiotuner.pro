TARGET = tst_qradiotuner
INCLUDEPATH += ../../../src/multimedia

CONFIG += testcase
symbian {
    HEADERS += tst_qradiotuner_s60.h
    SOURCES += tst_qradiotuner_s60.cpp
}
HEADERS += tst_qradiotuner.h
SOURCES += main.cpp tst_qradiotuner.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

