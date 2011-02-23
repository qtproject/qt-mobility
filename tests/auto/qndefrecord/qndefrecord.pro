SOURCES += tst_qndefrecord.cpp
TARGET = tst_qndefrecord
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
