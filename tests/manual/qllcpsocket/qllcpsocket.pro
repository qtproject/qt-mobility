SOURCES += tst_qllcpsocket.cpp
TARGET = tst_qllcpsocket
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

symbian:TARGET.CAPABILITY = LocalServices
