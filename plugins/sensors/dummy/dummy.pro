INCLUDEPATH+=../../../src/sensors

TEMPLATE = lib
CONFIG += plugin
PLUGIN_TYPE = sensors

include(version.pri)
include(dummy.pri)
include(../../../common.pri)


TARGET = $$qtLibraryTarget(qtsensors_dummy)

QT=core
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags
