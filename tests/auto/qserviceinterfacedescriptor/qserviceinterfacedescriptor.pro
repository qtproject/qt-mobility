TARGET = tst_qserviceinterfacedescriptor
INCLUDEPATH += ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qserviceinterfacedescriptor.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
