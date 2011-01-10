HEADERS  +=
TARGET = tst_qsystemalignedtimer
CONFIG+=testcase

QT = core network

SOURCES  += tst_qsystemalignedtimer.cpp
INCLUDEPATH += ../../../src/systeminfo

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}


