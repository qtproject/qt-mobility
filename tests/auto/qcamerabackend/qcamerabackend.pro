TARGET = tst_qcamerabackend
INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/video
CONFIG += testcase

SOURCES += tst_qcamerabackend.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
