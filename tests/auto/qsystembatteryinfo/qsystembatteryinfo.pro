TARGET = tst_qsystembatteryinfo
CONFIG+=testcase

SOURCES  += tst_qsystembatteryinfo.cpp
QT = core 
DEFINES += TESTR
INCLUDEPATH += ../../../src/systeminfo

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
