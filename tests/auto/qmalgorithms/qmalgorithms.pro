QT += testlib
TEMPLATE=app
TARGET=tst_qmalgorithms
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

INCLUDEPATH += ../../../src/global
INCLUDEPATH += ../

CONFIG += mobility
SOURCES  += tst_qmalgorithms.cpp
