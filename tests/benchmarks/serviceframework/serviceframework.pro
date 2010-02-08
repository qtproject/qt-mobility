TARGET = tst_bm_serviceframework
CONFIG+=testcase

SOURCES  += tst_serviceframework.cpp
QT = core network xml gui sql dbus
INCLUDEPATH += ../../../src/serviceframework

include(../../../common.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = All -TCB
}
