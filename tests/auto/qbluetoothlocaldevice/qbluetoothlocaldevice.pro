SOURCES += tst_qbluetoothlocaldevice.cpp
TARGET=tst_qbluetoothlocaldevice
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

symbian: TARGET.CAPABILITY = ReadDeviceData LocalServices WriteDeviceData NetworkControl
