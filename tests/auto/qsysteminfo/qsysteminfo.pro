SOURCES  += tst_qsysteminfo.cpp
HEADERS  += 
TARGET = tst_qsysteminfo
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
