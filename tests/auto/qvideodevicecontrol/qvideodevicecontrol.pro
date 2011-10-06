INCLUDEPATH += \
        ../../../src/multimedia

CONFIG += testcase

TARGET = tst_qvideodevicecontrol
SOURCES += tst_qvideodevicecontrol.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}

