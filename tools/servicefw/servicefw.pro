TEMPLATE = app
TARGET = servicefw
CONFIG += console mobility
MOBILITY = serviceframework
DEFINES += IGNORE_SERVICEMETADATA_EXPORT
INCLUDEPATH += ../../src/serviceframework

SOURCES = servicefw.cpp \
          ../../src/serviceframework/servicemetadata.cpp
HEADERS += ../../src/serviceframework/servicemetadata_p.h

include(../../common.pri)

include(../../features/deploy.pri)
