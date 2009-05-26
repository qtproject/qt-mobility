TEMPLATE=app
TARGET=servicefw

INCLUDEPATH += ../../servicefw/serviceframework/servicehandler \
               ../../servicefw/serviceframework/serviceresolution \
               ../../servicefw/serviceframework/

SOURCES=servicefw.cpp

include(../../common.pri)
LIBS += -lQtServiceFramework
