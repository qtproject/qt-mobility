TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_grueplugin)
PLUGIN_TYPE = sensors

INCLUDEPATH+=../../../src/sensors

include(version.pri)
include(grueplugin.pri)
include(../../examples.pri)

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB    
}

QT=core
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags
