SOURCES += tst_qbluetoothdeviceinfo.cpp
TARGET=tst_qbluetoothdeviceinfo
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
