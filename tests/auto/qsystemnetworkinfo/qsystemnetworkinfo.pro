TARGET = tst_qsystemnetworkinfo
CONFIG+=testcase

QT = core network
SOURCES  += tst_qsystemnetworkinfo.cpp

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
