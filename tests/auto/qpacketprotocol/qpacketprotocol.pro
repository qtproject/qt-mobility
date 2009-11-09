TARGET = tst_qpacketprotocol
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context
CONFIG += testcase
QT = core network
include(../../../common.pri)

# Input
SOURCES += tst_qpacketprotocol.cpp

qtAddLibrary(QtPublishSubscribe)
symbian:TARGET.CAPABILITY = ALL \
    -TCB
