TEMPLATE = app
TARGET = servicedbgen
CONFIG += console mobility
MOBILITY = serviceframework
QT += sql
QT -= gui

DEFINES += SERVICE_XML_GENERATOR
DEFINES += QT_SFW_SERVICEDATABASE_GENERATE
INCLUDEPATH += ../../src/serviceframework

PUBLIC_HEADERS += \
            ../../src/serviceframework/qservice.h \
            ../../src/serviceframework/qservicemanager.h \
            ../../src/serviceframework/qserviceplugininterface.h \
            ../../src/serviceframework/qservicecontext.h \
            ../../src/serviceframework/qabstractsecuritysession.h \
            ../../src/serviceframework/qserviceinterfacedescriptor.h \
            ../../src/serviceframework/qservicefilter.h

PRIVATE_HEADERS += ../../src/serviceframework/servicemetadata_p.h \
            ../../src/serviceframework/qserviceinterfacedescriptor_p.h \
            ../../src/serviceframework/dberror_p.h
PRIVATE_HEADERS +=  ../../src/serviceframework/servicedatabase_p.h \
            ../../src/serviceframework/databasemanager_p.h

SOURCES += servicedbgen.cpp \
          ../../src/serviceframework/servicemetadata.cpp \
          ../../src/serviceframework/qservicemanager.cpp \
          ../../src/serviceframework/qserviceplugininterface.cpp \
          ../../src/serviceframework/qservicecontext.cpp \
          ../../src/serviceframework/qabstractsecuritysession.cpp \
          ../../src/serviceframework/qserviceinterfacedescriptor.cpp \
          ../../src/serviceframework/qservicefilter.cpp \
          ../../src/serviceframework/servicedatabase.cpp \
          ../../src/serviceframework/databasemanager.cpp \
          ../../src/serviceframework/dberror.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

include(../../common.pri)
include(../../features/deploy.pri)

