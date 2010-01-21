INCLUDEPATH+=../../../src/sensors

include(version.pri)
include(generic.pri)
include(../../../common.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(sensors_generic)

QT=core
CONFIG+=mobility
MOBILITY+=sensors
QMAKE_CXXFLAGS+=-Werror
QMAKE_LFLAGS+=-Wl,-no-undefined

MOC_DIR = .moc/
OBJECTS_DIR = .obj/

DESTDIR = $$OUTPUT_DIR/bin/examples/sensors
target.path = $$SOURCE_DIR/plugins/sensors
INSTALLS += target

