TEMPLATE = app
TARGET = servicefw
CONFIG += console mobility
MOBILITY = serviceframework

INCLUDEPATH += ../../src/serviceframework

HEADERS = ../../src/serviceframework/servicemetadata_p.h
SOURCES = ../../src/serviceframework/servicemetadata.cpp \
          servicefw.cpp

include(../../common.pri)

include(../../features/deploy.pri)
