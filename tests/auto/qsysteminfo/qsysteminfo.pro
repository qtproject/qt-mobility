TARGET = tst_qsysteminfo
CONFIG+=testcase

SOURCES  += tst_qsysteminfo.cpp
QT = core network
contains(test_sim, yes) {
DEFINES += TESTR
}
INCLUDEPATH += ../../../src/systeminfo

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
