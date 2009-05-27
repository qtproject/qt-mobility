TEMPLATE=app
TARGET=servicefw
CONFIG+=console

INCLUDEPATH += ../../servicefw/serviceframework/servicehandler \
               ../../servicefw/serviceframework/serviceresolution \
               ../../servicefw/serviceframework/

SOURCES=servicefw.cpp

include(../../common.pri)
LIBS += -lQtServiceFramework
