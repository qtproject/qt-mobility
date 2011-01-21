QT += testlib
TEMPLATE = app
TARGET = tst_bm_algorithms

CONFIG += testcase

SOURCES += tst_bm_algorithms.cpp

INCLUDEPATH += ../../../src/global

include(../../../common.pri)

CONFIG += mobility
