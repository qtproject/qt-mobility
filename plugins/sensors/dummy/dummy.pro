INCLUDEPATH+=../../../src/sensors

include(version.pri)
include(dummy.pri)
include(../../../common.pri)

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_dummy)
symbian:TARGET.EPOCALLOWDLLDATA = 1

QT=core
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target

