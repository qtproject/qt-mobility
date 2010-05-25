TEMPLATE = app
TARGET = servicedbgen
CONFIG += console
QT += sql
QT -= gui

DEFINES += SERVICE_XML_GENERATOR
DEFINES += QT_SFW_SERVICEDATABASE_GENERATE
DEFINES += QT_SFW_SERVICEDATABASE_USE_SECURITY_TOKEN
#DEFINES += QT_SFW_SERVICEDATABASE_DEBUG
DEFINES += QT_BUILD_INTERNAL

# for Symbian^3 we don't need to change the windows path
DEFINES += QT_SYMBIAN_EMULATOR_SUPPORTS_PERPROCESS_WSD

INCLUDEPATH += ../../src/serviceframework

HEADERS += ../../src/serviceframework/servicedatabase_p.h

SOURCES += servicedbgen.cpp \
          ../../src/serviceframework/servicemetadata.cpp \
          ../../src/serviceframework/qserviceinterfacedescriptor.cpp \
          ../../src/serviceframework/dberror.cpp \
          ../../src/serviceframework/servicedatabase.cpp \
          ../../src/serviceframework/qservicefilter.cpp


include(../../common.pri)
include(../../features/deploy.pri)
