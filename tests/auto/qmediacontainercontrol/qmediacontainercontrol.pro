INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio ../qmultimedia_common
CONFIG += testcase

TARGET = tst_qmediacontainercontrol

SOURCES += tst_qmediacontainercontrol.cpp

include (../../../common.pri)
include (../qmultimedia_common/mockcontainer.pri)

CONFIG += mobility
MOBILITY = multimedia

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x3000000
}
