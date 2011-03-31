SOURCES += tst_ql2capserver.cpp
TARGET = tst_ql2capserver
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/bluetooth
DEPENDPATH += ../../../src/connectivity/bluetooth

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
symbian: TARGET.CAPABILITY =  LocalServices NetworkControl
