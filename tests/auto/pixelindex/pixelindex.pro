TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(qtgeoservices_pixelindexplugin)
PLUGIN_TYPE=geoservices

include(../../../common.pri)

CONFIG += mobility
MOBILITY = location

HEADERS += pixelindexengine.h

SOURCES += pixelindexengine.cpp \
           plugin.cpp


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
