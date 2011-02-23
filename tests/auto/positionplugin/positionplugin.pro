TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(qtposition_testplugin)
PLUGIN_TYPE=position

include(../../../common.pri)

CONFIG += mobility
MOBILITY = location

SOURCES += plugin.cpp

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps \
                ../../../src/location/maps/tiled


symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep      
}
