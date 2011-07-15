TARGET = tst_qcameraimagecapture
INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/video
CONFIG += testcase

SOURCES += \
    tst_qcameraimagecapture.cpp


include (../../../common.pri)
include (../qmultimedia_common/mock.pri)
include (../qmultimedia_common/mockcamera.pri)

CONFIG += mobility
MOBILITY = multimedia

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
