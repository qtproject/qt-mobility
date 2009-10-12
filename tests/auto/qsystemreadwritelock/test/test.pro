TARGET = tst_qsystemreadwritelock
INCLUDEPATH += ../../../../context
DEPENDPATH += ../../../../context

CONFIG+=testcase

QT = core

include(../../../../common.pri)

# Input 
SOURCES += ../tst_qsystemreadwritelock.cpp

LIBS += -lQtPublishSubscribe

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
