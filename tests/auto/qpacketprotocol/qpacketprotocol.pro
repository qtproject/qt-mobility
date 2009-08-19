TARGET = tst_qpacketprotocol
INCLUDEPATH += ../../../context
CONFIG += testcase
QT = core network
include(../../../common.pri)

# Input
SOURCES += tst_qpacketprotocol.cpp

LIBS += -lQtContextFramework
symbian:TARGET.CAPABILITY = ALL \
    -TCB
