SOURCES += tst_qbluetoothsocket.cpp
TARGET = tst_qbluetoothsocket
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

INCLUDEPATH += ../../../tests/btclient
DEPENDPATH += ../../../tests/btclient

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
