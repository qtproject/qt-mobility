TEMPLATE=app
TARGET=servicefw
CONFIG+=console

INCLUDEPATH += ../../src/serviceframework

SOURCES=servicefw.cpp

include(../../common.pri)

include(../../features/deploy.pri)
qtAddLibrary(QtServiceFramework)
