TEMPLATE = lib
TARGET = SFWDatabaseManagerServer
QT = core sql

DEFINES += QT_BUILD_SFW_LIB QT_MAKEDLL
DEFINES += DATABASEMANAGERSERVER_LIBRARY

DEPENDPATH += ../..

include(../../../../common.pri)


DEPENDPATH += ..

HEADERS +=  servicemetadata_p.h \
            servicedatabase_p.h \
            databasemanager_p.h \
            qserviceplugininterface.h \
            qabstractsecuritysession.h \
            qserviceinterfacedescriptor.h \
            qserviceinterfacedescriptor_p.h \
            qservicefilter.h \
            dberror_p.h \
            databasemanagerserver.h \
            databasemanagersession.h \
            databasemanagersignalhandler.h

            #qserviceglobal.h \
            #qservicemanager.h \
            #qservicecontext.h \


SOURCES +=  servicemetadata.cpp \
            servicedatabase.cpp \
            databasemanager.cpp \
            qserviceplugininterface.cpp \
            qabstractsecuritysession.cpp \
            qserviceinterfacedescriptor.cpp \
            qservicefilter.cpp \
            dberror.cpp \
            databasemanagerserver.cpp \
            databasemanagersession.cpp \
            databasemanagersignalhandler.cpp

            #qservicemanager.cpp \
            #qservicecontext.cpp \


TARGET.CAPABILITY = ALL -TCB
