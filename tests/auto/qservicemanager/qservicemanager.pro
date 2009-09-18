TARGET = tst_qservicemanager
INCLUDEPATH += ../../../serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
HEADERS += ../../sampleserviceplugin/sampleserviceplugin.h \
           ../qsfwtestutil.h
SOURCES += tst_qservicemanager.cpp \
           ../../sampleserviceplugin/sampleserviceplugin.cpp \
           ../qsfwtestutil.cpp

qtAddLibrary(QtServiceFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
