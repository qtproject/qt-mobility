TEMPLATE = app
TARGET = tst_bm_qsysteminfo
CONFIG += testcase

SOURCES += tst_qsysteminfo.cpp

QT += core

INCLUDEPATH += ../../../src/systeminfo

include(../../../common.pri)

CONFIG += mobility

MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB
}
