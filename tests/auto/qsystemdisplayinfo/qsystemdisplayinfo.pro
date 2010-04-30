TARGET = tst_qsystemdisplayinfo
CONFIG+=testcase

SOURCES  += tst_qsystemdisplayinfo.cpp
QT = core network gui

INCLUDEPATH += ../../../src/systeminfo

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
