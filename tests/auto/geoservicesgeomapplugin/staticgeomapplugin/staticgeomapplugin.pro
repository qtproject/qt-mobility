TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtgeoservices_staticgeomapplugin)
PLUGIN_TYPE=geoservices

include(../../../../common.pri)

CONFIG += mobility static
MOBILITY = location

HEADERS += qgeomappingmanagerengine_test.h \
           qgeoserviceproviderplugin_test.h

SOURCES += qgeoserviceproviderplugin_test.cpp


INCLUDEPATH += $$SOURCE_DIR/src/location \
               $$SOURCE_DIR/src/location/maps

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep      
}

target.path=$$QT_MOBILITY_PREFIX/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
