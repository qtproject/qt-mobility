TEMPLATE = lib
CONFIG += plugin static testplugin
TARGET = $$qtLibraryTarget(qtgeoservices_staticrouteplugin)
PLUGIN_TYPE=geoservices

include(../../../../common.pri)

CONFIG += mobility
MOBILITY = location

HEADERS += qgeoroutingmanagerengine_test.h \
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
