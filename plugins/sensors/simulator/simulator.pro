INCLUDEPATH+=../../../src/sensors

PLUGIN_TYPE = sensors

include(version.pri)
include(simulator.pri)

TEMPLATE = lib
TARGET = $$qtLibraryTarget(qtsensors_simulator)
CONFIG += plugin
include(../../../common.pri)
qtAddLibrary(QtMobilitySimulator)
symbian:TARGET.EPOCALLOWDLLDATA = 1

INCLUDEPATH += ../../../src/mobilitysimulator

QT=core gui network
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags

