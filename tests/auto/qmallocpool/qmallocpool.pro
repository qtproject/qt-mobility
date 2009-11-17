TARGET = tst_qmallocpool
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe
CONFIG += testcase
QT = core
include(../../../common.pri)

# Input
SOURCES += tst_qmallocpool.cpp

qtAddLibrary(QtPublishSubscribe)
symbian:TARGET.CAPABILITY = ALL \
    -TCB
