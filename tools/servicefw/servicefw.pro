TEMPLATE=app
TARGET=servicefw
CONFIG+=console

INCLUDEPATH += ../../serviceframework

SOURCES=servicefw.cpp

include(../../common.pri)
qtAddLibrary(QtServiceFramework)

include(../../features/deploy.pri)
