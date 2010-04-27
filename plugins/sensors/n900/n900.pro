INCLUDEPATH+=../../../src/sensors

include(version.pri)
include(n900.pri)
include(../../../common.pri)

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_n900)

QT=core
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target

BUILD_ALL_PLUGINS=$$(BUILD_ALL_PLUGINS)
equals(BUILD_ALL_PLUGINS,1) {
    DEFINES+=BUILD_ALL_PLUGINS
}

