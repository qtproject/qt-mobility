TEMPLATE=app
TARGET=accel_perf
include(../../examples.pri)
QT=core testlib
#CONFIG+=release
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors
SOURCES=main.cpp

CONFIG+=strict_flags

