SOURCES += tst_qllcpserver.cpp
TARGET = tst_qllcpserver
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

symbian:TARGET.CAPABILITY = LocalServices
