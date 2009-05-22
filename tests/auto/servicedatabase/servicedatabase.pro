TARGET=tst_servicedatabase
QT = core sql
INCLUDEPATH += ../../../servicefw/serviceframework/servicemetadata \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework/servicedatabase \
               ../../../servicefw/serviceframework

DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
CONFIG+=testcase

include(../../../common.pri)

# Input 
SOURCES +=  tst_servicedatabasetest.cpp \
            ../../../servicefw/serviceframework/servicemetadata/servicemetadata.cpp \
            ../../../servicefw/serviceframework/serviceresolution/qserviceinterfacedescriptor.cpp \
            ../../../servicefw/serviceframework/serviceresolution/qservicefilter.cpp \
            ../../../servicefw/serviceframework/serviceresolution/serviceinfo.cpp \
            ../../../servicefw/serviceframework/servicedatabase/servicedatabase.cpp 
HEADERS +=  ../../../servicefw/serviceframework/servicemetadata/servicemetadata_p.h \
            ../../../servicefw/serviceframework/serviceresolution/qserviceinterfacedescriptor.h \
            ../../../servicefw/serviceframework/serviceresolution/qserviceinterfacedescriptor_p.h \
            ../../../servicefw/serviceframework/serviceresolution/qservicefilter.h \
            ../../../servicefw/serviceframework/serviceresolution/serviceinfo_p.h \
            ../../../servicefw/serviceframework/servicedatabase/servicedatabase.h


#LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

