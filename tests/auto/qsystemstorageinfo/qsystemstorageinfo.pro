HEADERS  += 
TARGET = tst_qsystemstorageinfo
CONFIG+=testcase

QT = core network

SOURCES  += tst_qsystemstorageinfo.cpp
INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
qtAddLibrary(QtSystemInfo)

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}


