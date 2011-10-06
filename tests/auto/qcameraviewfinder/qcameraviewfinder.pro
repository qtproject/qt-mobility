CONFIG += testcase

TARGET = tst_qcameraviewfinder
SOURCES += tst_qcameraviewfinder.cpp

include (../../../common.pri)

INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/camera

CONFIG += mobility
MOBILITY = multimedia

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
