SOURCES  += tst_qnetworkconfigmanager.cpp
HEADERS  += ../qbearertestcommon.h
TARGET = tst_qnetworkconfigurationmanager
CONFIG+=testcase

QT = core network

INCLUDEPATH += ../../../bearer

include(../../../common.pri)
LIBS += -lQtBearer

symbian {
    TARGET.CAPABILITY = All -TCB -DRM 
}
