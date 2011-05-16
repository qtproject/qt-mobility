TARGET = tst_qmediabindableinterface

INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/audio
CONFIG += testcase

SOURCES += \
    tst_qmediabindableinterface.cpp

include (../../../common.pri)
include (../qmultimedia_common/mock.pri)
include (../qmultimedia_common/mockrecorder.pri)

CONFIG += mobility
MOBILITY = multimedia

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
