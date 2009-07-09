SOURCES  += tst_qnetworksession.cpp
HEADERS  += ../qbearertestcommon.h
TARGET = tst_qnetworksession
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../bearer

include(../../../common.pri)
LIBS += -lbearer

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
