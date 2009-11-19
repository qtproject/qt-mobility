HEADERS  += 
TARGET = tst_qsystemstorageinfo
CONFIG+=testcase

QT = core network

SOURCES  += tst_qsystemstorageinfo.cpp
INCLUDEPATH += ../../../src/systeminfo

include(../../../common.pri)
qtAddLibrary(QtSystemInfo)

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}


