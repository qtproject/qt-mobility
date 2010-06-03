INCLUDEPATH+=../../../src/sensors

include(version.pri)
include(generic.pri)
include(../../../common.pri)

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_generic)

symbian {
   TARGET.EPOCALLOWDLLDATA = 1
   TARGET.UID3 = 0x2002BFC3
   TARGET.CAPABILITY = ALL -TCB

   pluginDep.sources = $${TARGET}.dll
   pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
   DEPLOYMENT += pluginDep
}

QT=core
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags

DESTDIR = $$OUTPUT_DIR/bin/examples/sensors
target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target

