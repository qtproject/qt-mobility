TARGET = tst_qpacket
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context
CONFIG += testcase
QT = core
include(../../../common.pri)

# Input
SOURCES += tst_qpacket.cpp

qtAddLibrary(QtPublishSubscribe)
symbian:TARGET.CAPABILITY = ALL -TCB
