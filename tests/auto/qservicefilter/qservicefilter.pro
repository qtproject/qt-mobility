TEMPLATE = app
TARGET = tst_qservicefilter
INCLUDEPATH += ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework

CONFIG+=testcase

QT -= gui
QT+=testlib

include(../../../common.pri)

# Input 
SOURCES += tst_qservicefilter.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
}
