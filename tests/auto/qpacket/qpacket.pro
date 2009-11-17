TARGET = tst_qpacket
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe
CONFIG += testcase
QT = core
include(../../../common.pri)

# Input
SOURCES += tst_qpacket.cpp

qtAddLibrary(QtPublishSubscribe)
symbian:TARGET.CAPABILITY = ALL -TCB
