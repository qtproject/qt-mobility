TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtgeoservices_dummy)
PLUGIN_TYPE=geoservices

QT=core gui
CONFIG+=mobility
MOBILITY+=location

include(../../../common.pri)

HEADERS += qgeoplacesmanagerdummy.h \
           qgeomappingmanagerdummy.h \
           qgeoserviceproviderplugindummy.h

SOURCES += qgeoplacesmanagerdummy.cpp \
           qgeomappingmanagerdummy.cpp \
           qgeoserviceproviderplugindummy.cpp
           
RESOURCES += geoservicesdummymaptiles.qrc

!symbian {INCLUDEPATH += $$SOURCE_DIR/src/location }

symbian {
    TARGET.CAPABILITY = ALL -TCB
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep      
    TARGET.EPOCALLOWDLLDATA = 1
}

target.path=$$QT_MOBILITY_PREFIX/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
