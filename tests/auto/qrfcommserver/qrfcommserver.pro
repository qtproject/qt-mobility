SOURCES += tst_qrfcommserver.cpp
TARGET = tst_qrfcommserver
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
