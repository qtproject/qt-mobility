SOURCES += tst_qbluetoothtransfermanager.cpp
TARGET=tst_qbluetoothtransfermanager
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

INCLUDEPATH += ../../../tests/btclient
DEPENDPATH += ../../../tests/btclient

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

symbian: TARGET.CAPABILITY = All-TCB #ReadDeviceData LocalServices WriteDeviceData
