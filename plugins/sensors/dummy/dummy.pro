INCLUDEPATH+=../../../src/sensors

include(version.pri)
include(dummy.pri)
include(../../../common.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(sensors_dummy)
symbian:TARGET.EPOCALLOWDLLDATA = 1

QT=core
CONFIG+=mobility
MOBILITY+=sensors

STRICT=$$(STRICT)
equals(STRICT,1) {
    win32 {
        QMAKE_CXXFLAGS+=-WX
    } else {
        QMAKE_CXXFLAGS+=-Werror
        QMAKE_LFLAGS+=-Wl,-no-undefined
    }
}

DESTDIR = $$OUTPUT_DIR/bin/examples/sensors
target.path = $$SOURCE_DIR/plugins/sensors
INSTALLS += target

