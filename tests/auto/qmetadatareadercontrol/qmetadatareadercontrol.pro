CONFIG += testcase

TARGET = tst_qmetadatareadercontrol
SOURCES += tst_qmetadatareadercontrol.cpp

include (../../../common.pri)
include (../qmultimedia_common/mockcontainer.pri)

CONFIG += mobility
MOBILITY = multimedia

INCLUDEPATH += \
        ../../../src/multimedia

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
