

INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/camera

CONFIG += testcase

TARGET = tst_qvideoencodercontrol

SOURCES += \
    tst_qvideoencodercontrol.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
