TEMPLATE=app
TARGET=reading_perf
include(../../examples.pri)
QT=core testlib
#CONFIG+=release
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors
SOURCES=main.cpp

CONFIG+=strict_flags

