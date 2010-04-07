TEMPLATE = lib
TARGET = QtServiceFramework
QT = core sql

include(../../common.pri)

DEFINES += QT_BUILD_SFW_LIB QT_MAKEDLL

#include ipc extension
include(ipc/ipc.pri)

INCLUDEPATH += .

PUBLIC_HEADERS += \
            qservice.h \
            qservicemanager.h \
            qserviceplugininterface.h \
            qservicecontext.h \
            qabstractsecuritysession.h \
            qserviceinterfacedescriptor.h \
            qservicefilter.h \
            qservicecontrol.h \
            qservicetyperegister.h

PRIVATE_HEADERS += servicemetadata_p.h \
            qserviceinterfacedescriptor_p.h \
            dberror_p.h

SOURCES +=  servicemetadata.cpp \
            qservicemanager.cpp \
            qserviceplugininterface.cpp \
            qservicecontext.cpp \
            qabstractsecuritysession.cpp \
            qserviceinterfacedescriptor.cpp \
            qservicefilter.cpp \
            dberror.cpp \
            qservicecontrol.cpp \
            qservicetyperegister.cpp

symbian {
    INCLUDEPATH +=  ./symbian
    
    PRIVATE_HEADERS +=  databasemanager_s60_p.h

    SOURCES +=  databasemanager_s60.cpp

    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC84
    
    libBlock = \
        "$${LITERAL_HASH}ifdef WINSCW" \
        "LIBRARY SFWDatabaseManagerServer.lib" \
        "$${LITERAL_HASH}endif"

    MMP_RULES += libBlock

    QtServiceFrameworkDeployment.sources = QtServiceFramework.dll SFWDatabaseManagerServer.exe
    QtServiceFrameworkDeployment.path = /sys/bin

    DEPLOYMENT += QtServiceFrameworkDeployment
    DEFINES += QT_SFW_SERVICEDATABASE_USE_SECURITY_TOKEN

} else {
    PRIVATE_HEADERS +=  servicedatabase_p.h \
                databasemanager_p.h

    SOURCES +=  servicedatabase.cpp \
                databasemanager.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

CONFIG += middleware
include(../../features/deploy.pri)
