TARGET = tst_qmallocpool
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context
CONFIG += testcase
QT = core
include(../../../common.pri)

# Input
SOURCES += tst_qmallocpool.cpp

LIBS += -lQtPublishSubscribe
symbian:TARGET.CAPABILITY = ALL \
    -TCB
