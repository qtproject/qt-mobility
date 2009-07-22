SOURCES  += tst_qsystemdisplayinfo.cpp
HEADERS  += 
TARGET = tst_qsystemdisplayinfo
CONFIG+=testcase
QT += testlib
QT -= gui
QT = core network

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
