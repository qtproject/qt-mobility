TEMPLATE = app
TARGET = tst_bm_location
CONFIG += testcase

SOURCES += tst_bm_location.cpp

QT += core \
      network


INCLUDEPATH += ../../../src/location

include(../../../common.pri)

CONFIG += mobility

MOBILITY = location


symbian {
    TARGET.CAPABILITY = All -TCB
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    INCLUDEPATH += /epoc32/include/app
    INCLUDEPATH += /epoc32/include/platform/app
}
