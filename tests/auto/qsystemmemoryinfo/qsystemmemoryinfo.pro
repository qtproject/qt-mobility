load(qttest_p4)
SOURCES  += tst_qsystemmemoryinfo.cpp
HEADERS  += 
TARGET = tst_qsystemmemoryinfo
CONFIG+=testcase
QT -= gui

QT += core network

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}


