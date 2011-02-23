TEMPLATE = app
TARGET = servicedbgen
CONFIG += console
#CONFIG += static
QT += sql
QT -= gui

contains(CONFIG, static) {
   QTPLUGIN += qsqlite 
   DEFINES += STATIC_BUILD
}

DEFINES += SERVICE_XML_GENERATOR
DEFINES += QT_SFW_SERVICEDATABASE_GENERATE
DEFINES += QT_SFW_SERVICEDATABASE_USE_SECURITY_TOKEN
#DEFINES += QT_SFW_SERVICEDATABASE_DEBUG
DEFINES += QTM_BUILD_UNITTESTS

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

win32 {
   QMAKE_LFLAGS += -static-libgcc
}

