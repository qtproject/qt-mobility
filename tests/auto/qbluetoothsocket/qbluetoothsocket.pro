SOURCES += tst_qbluetoothsocket.cpp
TARGET = tst_qbluetoothsocket
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

INCLUDEPATH += ../../tools/btclient
DEPENDPATH += ../../tools/btclient

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
symbian: TARGET.CAPABILITY = NetworkControl ReadDeviceData LocalServices WriteDeviceData

OTHER_FILES += \
    README.txt
