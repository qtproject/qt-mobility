TARGET=tst_qservicecontext
INCLUDEPATH += ../../../src/serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
HEADERS += 
SOURCES += tst_qservicecontext.cpp 

CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
