TARGET = tst_qsystemreadwritelock
INCLUDEPATH += ../../../../src/publishsubscribe
DEPENDPATH += ../../../../src/publishsubscribe

CONFIG+=testcase

QT = core

include(../../../../common.pri)

# Input 
SOURCES += ../tst_qsystemreadwritelock.cpp

qtAddLibrary(QtPublishSubscribe)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
