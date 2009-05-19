TEMPLATE = app
TARGET = tst_qserviceinterfacedescriptor
INCLUDEPATH += ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework

CONFIG+=testcase

QT -= gui
QT+=testlib 

include(../../../common.pri)

# Input 
SOURCES += tst_qserviceinterfacedescriptor.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
