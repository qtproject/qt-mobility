TEMPLATE=app
TARGET=accel
include(../../examples.pri)
QT=core
CONFIG+=mobility
MOBILITY+=sensors
SOURCES=main.cpp
QMAKE_CXXFLAGS+=-Werror
