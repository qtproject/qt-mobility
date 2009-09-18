TARGET = tst_qvaluespace
INCLUDEPATH += ../../../context

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qvaluespace.cpp

qtAddLibrary(QtContextFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
