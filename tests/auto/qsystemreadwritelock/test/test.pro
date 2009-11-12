TARGET = tst_qsystemreadwritelock
INCLUDEPATH += ../../../../context
DEPENDPATH += ../../../../context

CONFIG+=testcase

QT = core

include(../../../../common.pri)

# Input 
SOURCES += ../tst_qsystemreadwritelock.cpp

qtAddLibrary(QtPublishSubscribe)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
