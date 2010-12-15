TARGET = tst_qsystembatteryinfo
CONFIG+=testcase

SOURCES  += tst_qsystembatteryinfo.cpp
QT = core 

INCLUDEPATH += ../../../src/systeminfo

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}

contains(test_use_sim, yes) {
    DEFINES += TESTR QT_SIMULATOR
}
