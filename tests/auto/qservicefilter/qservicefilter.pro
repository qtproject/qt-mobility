TARGET=tst_qservicefilter
SOURCES += tst_qservicefilter.cpp
INCLUDEPATH += ../../../src/serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
