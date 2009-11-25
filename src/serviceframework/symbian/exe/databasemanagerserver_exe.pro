TEMPLATE = app
TARGET = SFWDatabaseManagerServer
QT = core sql
TARGET.UID3 = 0xE3b48c24

DEFINES += SYMBIAN_DATABASEMANAGER_SERVER
DEFINES += DATABASEMANAGERSERVER_NO_LIBRARY

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
            
TARGET.CAPABILITY = ALL -TCB
