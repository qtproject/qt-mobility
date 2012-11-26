INCLUDEPATH+=../../../src/sensors

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_blackberry)

include(../../../common.pri)
include(blackberry.pri)

QT = core
CONFIG += mobility
MOBILITY += sensors
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

contains(bbsensor_header_enabled, yes):DEFINES += HAVE_NDK_SENSOR_H
contains(bbsensor_holster_enabled, yes):DEFINES += HAVE_HOLSTER_SENSOR
contains(bbsensor_compass_enabled, yes):DEFINES += HAVE_COMPASS_SENSOR
