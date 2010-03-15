TEMPLATE=app
TARGET=accel
include(../../examples.pri)
QT=core
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors
SOURCES=main.cpp

CONFIG+=strict_flags

