TEMPLATE = lib
TARGET = QtServiceFramework
QT = core sql

include(../../common.pri)

DEFINES += QT_BUILD_SFW_LIB QT_MAKEDLL

PUBLIC_HEADERS += \
            qservice.h \
            qservicemanager.h \
            qserviceplugininterface.h \
            qservicecontext.h \
            qabstractsecuritysession.h \
            qserviceinterfacedescriptor.h \
            qservicefilter.h

PRIVATE_HEADERS += servicedatabase_p.h \
            databasemanager_p.h \
            servicemetadata_p.h \
            qserviceinterfacedescriptor_p.h \
            dberror_p.h

SOURCES +=  servicemetadata.cpp \
            qservicemanager.cpp \
            qserviceplugininterface.cpp \
            qservicecontext.cpp \
            qabstractsecuritysession.cpp \
            qserviceinterfacedescriptor.cpp \
            qservicefilter.cpp \
            dberror.cpp

symbian {
    INCLUDEPATH +=  ./databasemanagerserver_symbian
    
    PRIVATE_HEADERS +=  databasemanager_symbian_p.h

    SOURCES +=  databasemanager_symbian.cpp

    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC84

    QtServiceFrameworkDeployment.sources = QtServiceFramework.dll qsfwdatabasemanagerserver.exe
    QtServiceFrameworkDeployment.path = /sys/bin

    DEPLOYMENT += QtServiceFrameworkDeployment

} else {
    SOURCES +=  servicedatabase.cpp \
                databasemanager.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

CONFIG += middleware
include(../../features/deploy.pri)
