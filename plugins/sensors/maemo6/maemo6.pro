INCLUDEPATH+=../../../src/sensors

include(version.pri)
include(maemo6.pri)
include(../../../common.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_maemo6)

QT = core dbus network
CONFIG += mobility
MOBILITY += sensors
CONFIG += link_pkgconfig
PKGCONFIG += sensord

DESTDIR = $$OUTPUT_DIR/bin/examples/sensors
target.path = $$SOURCE_DIR/plugins/sensors
INSTALLS += target

