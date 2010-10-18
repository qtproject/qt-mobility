QT += testlib
TEMPLATE=app
TARGET=tst_qlatin1constant
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

INCLUDEPATH += ../../../src/contacts

INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = contacts

SOURCES  += tst_qlatin1constant.cpp

