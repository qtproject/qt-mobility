TARGET = tst_qcamera
INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/video
CONFIG += testcase

SOURCES += tst_qcamera.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

symbian { 
    TARGET.CAPABILITY = ALL -TCB 
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000 
}
