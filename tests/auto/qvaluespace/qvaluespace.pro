TARGET = tst_qvaluespace
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qvaluespace.cpp

LIBS += -lQtPublishSubscribe

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
