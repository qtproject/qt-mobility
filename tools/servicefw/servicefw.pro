TEMPLATE=app
TARGET=servicefw
CONFIG+=console mobility
MOBILITY = serviceframework

INCLUDEPATH += ../../src/serviceframework

SOURCES=servicefw.cpp

include(../../common.pri)

include(../../features/deploy.pri)
