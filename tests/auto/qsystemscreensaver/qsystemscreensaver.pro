SOURCES  += tst_qsystemscreensaver.cpp
TARGET = tst_qsystemscreensaver
CONFIG+=testcase
QT += testlib
QT = gui core network

INCLUDEPATH += ../../../systeminfo

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
    }

