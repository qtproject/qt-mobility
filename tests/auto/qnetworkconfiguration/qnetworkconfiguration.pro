SOURCES  += tst_qnetworkconfiguration.cpp
HEADERS  += ../qbearertestcommon.h
TARGET = tst_qnetworkconfiguration
CONFIG+=testcase

QT = core network

INCLUDEPATH += ../../../bearer

include(../../../common.pri)
LIBS += -lbearer

symbian {
    TARGET.CAPABILITY = All -TCB -DRM 
}
