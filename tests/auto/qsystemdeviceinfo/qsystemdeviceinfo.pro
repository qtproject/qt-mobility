TARGET = tst_qsystemdeviceinfo
CONFIG+=testcase
QT = core network

INCLUDEPATH += ../../../src/systeminfo
SOURCES  += tst_qsystemdeviceinfo.cpp
include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
message($$test_use_sim)
contains(test_use_sim, yes) {
    DEFINES += TESTR QT_SIMULATOR
}
