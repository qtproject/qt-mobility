#include(../../../../common.pri)

QT += gui
CONFIG += qtestlib \
    mobility
MOBILITY = location

INCLUDEPATH+=../../../../src/location/maps ..

HEADERS+= ../testhelper.h \
    ../testsuite.h

SOURCES+= ../testhelper.cpp  ../main.cpp