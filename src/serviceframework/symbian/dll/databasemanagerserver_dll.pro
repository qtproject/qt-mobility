TEMPLATE = lib
TARGET = SFWDatabaseManagerServer
QT = core sql

TARGET.UID3 = 0x2002BFBE

DEFINES += QT_BUILD_SFW_LIB QT_MAKEDLL
DEFINES += DATABASEMANAGERSERVER_LIBRARY
DEFINES += QT_SFW_SERVICEDATABASE_USE_SECURITY_TOKEN

DEPENDPATH += ../..

include(../../../../common.pri)
# override the defFilePath set by common.pri
defFilePath=../../../s60installs


DEPENDPATH += ..

HEADERS +=  servicemetadata_p.h \
            servicedatabase_p.h \
            qserviceplugininterface.h \
            qabstractsecuritysession.h \
            qserviceinterfacedescriptor.h \
            qserviceinterfacedescriptor_p.h \
            qservicefilter.h \
            dberror_p.h \
            databasemanagerserver.h \
            databasemanagersession.h \
            databasemanagersignalhandler.h

SOURCES +=  servicemetadata.cpp \
            servicedatabase.cpp \
            qserviceplugininterface.cpp \
            qabstractsecuritysession.cpp \
            qserviceinterfacedescriptor.cpp \
            qservicefilter.cpp \
            dberror.cpp \
            databasemanagerserver.cpp \
            databasemanagersession.cpp \
            databasemanagersignalhandler.cpp

TARGET.CAPABILITY = ALL -TCB
