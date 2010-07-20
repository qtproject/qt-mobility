INCLUDEPATH+=../../../src/sensors

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_n900)

include(version.pri)
include(n900.pri)
include(../../../common.pri)

QT=core
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags

BUILD_ALL_PLUGINS=$$(BUILD_ALL_PLUGINS)
equals(BUILD_ALL_PLUGINS,1) {
    DEFINES+=BUILD_ALL_PLUGINS
}

