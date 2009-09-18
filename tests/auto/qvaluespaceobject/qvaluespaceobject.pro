TARGET = tst_qvaluespaceobject
INCLUDEPATH += ../../../context

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qvaluespaceobject.cpp

qtAddLibrary(QtContextFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
