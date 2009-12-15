TARGET = tst_qserviceinterfacedescriptor
INCLUDEPATH += ../../../src/serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qserviceinterfacedescriptor.cpp

CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
