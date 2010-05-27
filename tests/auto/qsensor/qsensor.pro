TEMPLATE = app
TARGET = tst_qsensor

CONFIG += testcase
QT += testlib

include(../../../common.pri)
#include(../support/support.pri)

CONFIG += mobility
MOBILITY = sensors
INCLUDEPATH += ../../../src/sensors

SOURCES += \
    tst_qsensor.cpp

HEADERS += \
    test_sensor.h\
    test_sensor_p.h\
    test_sensorimpl.h

SOURCES += \
    test_sensor.cpp\
    test_sensorimpl.cpp\
    test_sensorplugin.cpp

