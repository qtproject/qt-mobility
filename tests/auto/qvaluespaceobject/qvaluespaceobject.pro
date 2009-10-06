TARGET = tst_qvaluespaceobject
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qvaluespaceobject.cpp

qtAddLibrary(QtContextFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

win32 {
    !wince*:LIBS += -ladvapi32
}
