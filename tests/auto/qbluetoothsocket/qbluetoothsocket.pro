SOURCES += tst_qbluetoothsocket.cpp
TARGET = tst_qbluetoothsocket
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
