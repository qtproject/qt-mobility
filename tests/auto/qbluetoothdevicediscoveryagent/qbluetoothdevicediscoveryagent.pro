SOURCES += tst_qbluetoothdevicediscoveryagent.cpp
TARGET=tst_qbluetoothdevicediscoveryagent
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
