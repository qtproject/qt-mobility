TARGET = tst_qsystemdisplayinfo
CONFIG+=testcase

SOURCES  += tst_qsystemdisplayinfo.cpp
QT = core network gui

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
qtAddLibrary(QtSystemInfo)

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
