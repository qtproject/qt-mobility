HEADERS  +=
TARGET = tst_qsystemstorageinfo
CONFIG+=testcase

QT = core network

SOURCES  += tst_qsystemstorageinfo.cpp
INCLUDEPATH += ../../../src/systeminfo
DEFINES += TESTR
include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}


