SOURCES  += tst_qnetworkconfigmanager.cpp
TARGET = tst_qnetworkconfigurationmanager
CONFIG+=testcase

QT = core network

INCLUDEPATH += ../../../bearer

include(../../../common.pri)
LIBS += -lbearer

symbian {
    TARGET.CAPABILITY = All -TCB -DRM 
}
