SOURCES += tst_qbluetoothtransfermanager.cpp
TARGET=tst_qbluetoothtransfermanager
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

INCLUDEPATH += ../../tools/btclient
DEPENDPATH += ../../tools/btclient

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

symbian: TARGET.CAPABILITY = All-TCB #ReadDeviceData LocalServices WriteDeviceData
