TARGET = tst_qvaluespace
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qvaluespace.cpp

CONFIG += mobility
MOBILITY = publishsubscribe

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
