INCLUDEPATH+=../../../src/sensors

include(../../../common.pri)
include(s60_sensor_api.pri)
include(version.pri)

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(sensor_s60sensorapi)
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002BFC1
TARGET.CAPABILITY = ALL -TCB

QT=core
CONFIG+=mobility
MOBILITY+=sensors

pluginDep.sources = $${TARGET}.dll
pluginDep.path = $${QT_PLUGINS_BASE_DIR}/sensors
DEPLOYMENT += pluginDep

#Sensor API spesific deployment
QtSensorsDeployment.sources = QtSensors.dll
QtSensorsDeployment.path = /sys/bin
DEPLOYMENT += QtSensorsDeployment

target.path += $$[QT_INSTALL_PLUGINS]/sensors
INSTALLS += target
