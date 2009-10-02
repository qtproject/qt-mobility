TARGET = tst_qsystemreadwritelock_oop
INCLUDEPATH += ../../../../context

CONFIG+=testcase

QT = core

include(../../../../common.pri)

# Input 
SOURCES += ../tst_qsystemreadwritelock_oop.cpp

LIBS += -lQtContextFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
