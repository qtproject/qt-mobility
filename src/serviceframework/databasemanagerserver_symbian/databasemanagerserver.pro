include(../../../features/utils.pri)

TEMPLATE = app
TARGET = qsfwdatabasemanagerserver$${QT_LIBINFIX}
QT = core sql

CONFIG += no_icon

DEFINES += QTM_SERVICEFW_SYMBIAN_DATABASEMANAGER_SERVER
DEFINES += QT_SFW_SERVICEDATABASE_USE_SECURITY_TOKEN

SOURCES += databasemanagerservermain.cpp

include(../../../common.pri)

TARGET.UID3 = $$mobilityUID(0x2002AC7F)

DEPENDPATH += ../
INCLUDEPATH += ../

HEADERS +=  servicemetadata_p.h \
            servicedatabase_p.h \
            qserviceplugininterface.h \
            qabstractsecuritysession.h \
            qserviceinterfacedescriptor.h \
            qserviceinterfacedescriptor_p.h \
            qservicefilter.h \
            dberror_p.h \
            databasemanagerserver_p.h \
            databasemanagersession_p.h \
            databasemanagersignalhandler_p.h

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
