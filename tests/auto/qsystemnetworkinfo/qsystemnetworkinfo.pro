SOURCES  += tst_qsystemnetworkinfo.cpp
HEADERS  += 
TARGET = tst_qsystemnetworkinfo
CONFIG+=testcase
QT -= gui

QT = core network

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
