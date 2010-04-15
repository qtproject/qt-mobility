TEMPLATE = app
TARGET = SFWDatabaseManagerServer
QT = core sql
TARGET.UID3 = 0x2002AC7F

CONFIG += no_icon

DEFINES += SYMBIAN_DATABASEMANAGER_SERVER
DEFINES += DATABASEMANAGERSERVER_NO_LIBRARY
DEFINES += QT_SFW_SERVICEDATABASE_USE_SECURITY_TOKEN

SOURCES += databasemanagerservermain.cpp

include(../../../../common.pri)


DEPENDPATH += ../ ../..

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

#ProtServ is needed so that the server can be in protected namespace (start with '!' -mark).
TARGET.CAPABILITY = ProtServ
