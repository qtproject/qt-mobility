TARGET = tst_qsystemmemoryinfo
CONFIG+=testcase

QT = core network

SOURCES  += tst_qsystemmemoryinfo.cpp
INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}


