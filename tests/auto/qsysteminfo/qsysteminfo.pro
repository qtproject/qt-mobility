TARGET = tst_qsysteminfo
CONFIG+=testcase

SOURCES  += tst_qsysteminfo.cpp
QT = core network

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
