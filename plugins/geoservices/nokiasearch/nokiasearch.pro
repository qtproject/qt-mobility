TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtgeoservices_nokiasearch)
PLUGIN_TYPE=geoservices

include(../../../common.pri)

QT += network

CONFIG += mobility
MOBILITY = location

HEADERS += \
            qgeosearchreply_nokia.h \
            qgeosearchmanagerengine_nokia.h \
            qgeorouteparser.h \
            qgeoroutereply_nokia.h \
            qgeoroutingmanagerengine_nokia.h \
            qgeoserviceproviderplugin_nokia.h

SOURCES += \
            qgeosearchreply_nokia.cpp \
            qgeosearchmanagerengine_nokia.cpp \
            qgeorouteparser.cpp \
            qgeoroutereply_nokia.cpp \
            qgeoroutingmanagerengine_nokia.cpp \
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
