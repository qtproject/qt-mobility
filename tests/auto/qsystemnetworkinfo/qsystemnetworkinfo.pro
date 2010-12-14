TARGET = tst_qsystemnetworkinfo
CONFIG+=testcase

QT = core network
SOURCES  += tst_qsystemnetworkinfo.cpp

INCLUDEPATH += ../../../src/systeminfo
contains(test_sim, yes) {
DEFINES += TESTR QT_SIMULATOR
}
include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
