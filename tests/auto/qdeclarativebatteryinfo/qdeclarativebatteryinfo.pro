TARGET = tst_qdeclarativebatteryinfo
CONFIG+=testcase

SOURCES  += tst_qdeclarativebatteryinfo.cpp
QT = core 

HEADERS += ../../../plugins/declarative/systeminfo/qdeclarativebatteryinfo_p.h 
SOURCES += ../../../plugins/declarative/systeminfo/qdeclarativebatteryinfo.cpp 


INCLUDEPATH += ../../../src/systeminfo
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/systeminfo

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}

contains(test_use_sim, yes) {
DEFINES += TESTR QT_SIMULATOR
}
