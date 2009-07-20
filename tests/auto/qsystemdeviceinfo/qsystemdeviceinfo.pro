load(qttest_p4)
SOURCES  += tst_qsystemdeviceinfo.cpp
HEADERS  += 
TARGET = tst_qsystemdeviceinfo
CONFIG+=testcase
QT -= gui
QT += testlib

QT = core network

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
