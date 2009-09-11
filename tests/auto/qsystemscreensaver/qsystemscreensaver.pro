TARGET = tst_qsystemscreensaver
CONFIG+=testcase
QT = gui core network

INCLUDEPATH += ../../../systeminfo
SOURCES  += tst_qsystemscreensaver.cpp

include(../../../common.pri)
LIBS += -lQtSystemInfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}

