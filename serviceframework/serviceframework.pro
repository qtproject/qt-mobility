TEMPLATE = lib
TARGET = QtServiceFramework
QT = core sql

include(../common.pri)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

 

DEFINES += QT_BUILD_SFW_LIB QT_MAKEDLL

PUBLIC_HEADERS +=  qserviceglobal.h \
            qservicemanager.h \
            qserviceplugininterface.h \
            qservicecontext.h \
            qabstractsecuritysession.h \
            qserviceinterfacedescriptor.h \
            qservicefilter.h

PRIVATE_HEADERS += servicemetadata_p.h \
            qserviceinterfacedescriptor_p.h \
            databasemanager_p.h \
            servicedatabase_p.h \


HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES +=  servicemetadata.cpp \
            servicedatabase.cpp \
            databasemanager.cpp \
            qservicemanager.cpp \
            qserviceplugininterface.cpp \
            qservicecontext.cpp \
            qabstractsecuritysession.cpp \
            qserviceinterfacedescriptor.cpp \
            qservicefilter.cpp


include(../features/deploy.pri)
