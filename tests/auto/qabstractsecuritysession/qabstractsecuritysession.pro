TEMPLATE = app
TARGET = tst_qabstractsecuritysession
INCLUDEPATH += ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework/servicehandler \
               ../../../servicefw/serviceframework

CONFIG+=testcase

QT -= gui
QT+=testlib 

include(../../../common.pri)

# Input 
SOURCES += tst_qabstractsecuritysession.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
}
