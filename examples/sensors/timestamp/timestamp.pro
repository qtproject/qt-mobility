TEMPLATE=app
TARGET=timestamp
include(../../examples.pri)
QT=core testlib
#CONFIG+=release
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors
SOURCES=main.cpp

load(strict_flags)
QMAKE_CXXFLAGS -= -Werror

