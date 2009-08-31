SOURCES  += tst_qnetworksession.cpp
HEADERS  += ../qbearertestcommon.h
TARGET = tst_qnetworksession
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../bearer

include(../../../common.pri)
LIBS += -lQtBearer

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
}
