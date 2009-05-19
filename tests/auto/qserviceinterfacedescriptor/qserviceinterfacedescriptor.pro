TEMPLATE = app
TARGET = tst_qserviceinterfacedescriptor
INCLUDEPATH += ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework

DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
CONFIG+=testcase

QT -= gui
QT+=testlib 

include(../../../common.pri)

# Input 
SOURCES += tst_qserviceinterfacedescriptor.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
}
