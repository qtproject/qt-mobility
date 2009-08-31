TARGET = tst_qmallocpool
INCLUDEPATH += ../../../context
CONFIG += testcase
QT = core
include(../../../common.pri)

# Input
SOURCES += tst_qmallocpool.cpp

LIBS += -lQtContextFramework
symbian:TARGET.CAPABILITY = ALL \
    -TCB
