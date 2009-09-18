TARGET = tst_qserviceinterfacedescriptor
INCLUDEPATH += ../../../serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qserviceinterfacedescriptor.cpp

qtAddLibrary(QtServiceFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
