TARGET = tst_qservicemanager
INCLUDEPATH += ../../../servicefw/serviceframework/servicehandler \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
HEADERS += ../../sampleserviceplugin/sampleserviceplugin.h \
           ../qsfwtestutil.h
SOURCES += tst_qservicemanager.cpp \
           ../../sampleserviceplugin/sampleserviceplugin.cpp \
           ../qsfwtestutil.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
