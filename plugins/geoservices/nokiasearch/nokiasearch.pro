TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtgeoservices_nokiasearch)
PLUGIN_TYPE=geoservices

include(../../../common.pri)

QT += network

CONFIG += mobility
MOBILITY = location

HEADERS += \
            qgeoserviceproviderplugin_nokia.h

SOURCES += \
            qgeoserviceproviderplugin_nokia.cpp

INCLUDEPATH += $$QT_MOBILITY_BUILD_TREE/include/QtmLocation
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/src/location

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep      
}

target.path=$$QT_MOBILITY_PREFIX/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
