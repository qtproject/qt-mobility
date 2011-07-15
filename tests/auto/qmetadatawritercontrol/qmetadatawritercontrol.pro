CONFIG += testcase

TARGET = tst_qmetadatawritercontrol
SOURCES += tst_qmetadatawritercontrol.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

INCLUDEPATH += \
        ../../../src/multimedia

include (../qmultimedia_common/mockcontainer.pri)

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
