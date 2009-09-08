TEMPLATE=app
TARGET=servicefw
CONFIG+=console

INCLUDEPATH += ../../serviceframework

SOURCES=servicefw.cpp

include(../../common.pri)
LIBS += -lQtServiceFramework

include(../../features/deploy.pri)
