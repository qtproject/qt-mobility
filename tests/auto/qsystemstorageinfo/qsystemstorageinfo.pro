HEADERS  +=
TARGET = tst_qsystemstorageinfo
CONFIG+=testcase

QT = core network

SOURCES  += tst_qsystemstorageinfo.cpp
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
