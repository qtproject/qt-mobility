INCLUDEPATH+=../../../src/sensors

include(version.pri)
include(grueplugin.pri)
include(../../../common.pri)

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(sensors_grueplugin)
symbian:TARGET.EPOCALLOWDLLDATA = 1

QT=core
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags

target.path = $${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
maemo6:target.path=$$[QT_INSTALL_PLUGINS]/$${PLUGIN_TYPE}
INSTALLS += target
