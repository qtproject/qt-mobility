TARGET = tst_qsystemreadwritelock
INCLUDEPATH += ../../../../src/publishsubscribe
DEPENDPATH += ../../../../src/publishsubscribe

CONFIG+=testcase

QT = core

include(../../../../common.pri)

# Input 
SOURCES += ../tst_qsystemreadwritelock.cpp

CONFIG += mobility
MOBILITY = publishsubscribe

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
