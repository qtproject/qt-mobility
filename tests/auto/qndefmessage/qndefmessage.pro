SOURCES += tst_qndefmessage.cpp
TARGET = tst_qndefmessage
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc

include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity
