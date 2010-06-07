INCLUDEPATH+=../../../src/sensors

PLUGIN_TYPE = sensors

include(version.pri)
include(simulator.pri)
qtAddLibrary(QtMobilitySimulator)

TEMPLATE = lib
TARGET = $$qtLibraryTarget(qtsensors_simulator)
CONFIG += plugin
include(../../../common.pri)
symbian:TARGET.EPOCALLOWDLLDATA = 1

INCLUDEPATH += ../../../src/mobilitysimulator

QT=core gui network
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags

