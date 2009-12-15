TARGET = tst_qpacketprotocol
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe
CONFIG += testcase
QT = core network
include(../../../common.pri)

# Input
SOURCES += tst_qpacketprotocol.cpp

CONFIG += mobility
MOBILITY = publishsubscribe
symbian:TARGET.CAPABILITY = ALL \
    -TCB
