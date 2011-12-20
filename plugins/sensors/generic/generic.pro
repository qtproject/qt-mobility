include(../../../features/utils.pri)

INCLUDEPATH+=../../../src/sensors

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$mobilityPluginTarget(qtsensors_generic)

include(generic.pri)
include(../../../common.pri)

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = $$mobilityUID(0x2002BFC3)
    TARGET.CAPABILITY = ALL -TCB

    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep
}

QT=core
CONFIG+=mobility
MOBILITY+=sensors

CONFIG+=strict_flags
