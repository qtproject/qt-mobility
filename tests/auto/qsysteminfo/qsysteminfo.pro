TARGET = tst_qsysteminfo
CONFIG+=testcase

SOURCES  += tst_qsysteminfo.cpp
QT = core network

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
qtAddLibrary(QtSystemInfo)

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
