INCLUDEPATH+=../../../src/sensors

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_generic)

include(version.pri)
include(generic.pri)
include(../../../common.pri)

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
