TEMPLATE = app
TARGET = tst_qservicecontext
INCLUDEPATH += ../../../servicefw/serviceframework/servicehandler \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework

CONFIG+=testcase

QT -= gui
QT+=testlib 

include(../../../common.pri)

# Input 
HEADERS += 
SOURCES += tst_qservicecontext.cpp 

LIBS += -lQtServiceFramework

symbian {
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
}
