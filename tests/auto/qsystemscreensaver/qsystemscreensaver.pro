TARGET = tst_qsystemscreensaver
CONFIG+=testcase
QT = gui core network

INCLUDEPATH += ../../../systeminfo
SOURCES  += tst_qsystemscreensaver.cpp

include(../../../common.pri)
qtAddLibrary(QtSystemInfo)

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}

