TARGET = tst_qsystemdeviceinfo
CONFIG+=testcase
QT = core network

INCLUDEPATH += ../../../systeminfo
SOURCES  += tst_qsystemdeviceinfo.cpp

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
