TEMPLATE=app
TARGET=accel_perf
include(../../examples.pri)
QT=core testlib
CONFIG+=release
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors
SOURCES=main.cpp
STRICT=$$(STRICT)
equals(STRICT,1) {
    win32 {
        QMAKE_CXXFLAGS+=-WX
    } else {
        QMAKE_CXXFLAGS+=-Werror
        QMAKE_LFLAGS+=-Wl,-no-undefined
    }
}
