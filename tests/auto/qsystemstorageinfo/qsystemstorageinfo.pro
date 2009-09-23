SOURCES  += tst_qsystemstorageinfo.cpp
HEADERS  += 
TARGET = tst_qsystemstorageinfo
CONFIG+=testcase
QT -= gui

QT += core network

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}


