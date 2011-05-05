TARGET = tst_qsystemscreensaver
CONFIG+=testcase
QT = gui core network

INCLUDEPATH += ../../../src/systeminfo
SOURCES  += tst_qsystemscreensaver.cpp

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}

contains(test_use_sim, yes) {
DEFINES += TESTR QT_SIMULATOR
}
