TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtgeoservices_nokia)
PLUGIN_TYPE=geoservices

include(../../../common.pri)

QT += network

CONFIG += mobility
MOBILITY = location

HEADERS += \
            qgeocodexmlparser.h \
            qgeomappingmanagerengine_nokia.h \
            qgeomapreply_nokia.h \
            qgeoroutereply_nokia.h \
            qgeoroutexmlparser.h \
            qgeoroutingmanagerengine_nokia.h \
            qgeosearchmanagerengine_nokia.h \
            qgeosearchreply_nokia.h \
            qgeoserviceproviderplugin_nokia.h \
            marclanguagecodes.h \
            qgeotiledmapdata_nokia.h

SOURCES += \
            qgeocodexmlparser.cpp \
            qgeomappingmanagerengine_nokia.cpp \
            qgeomapreply_nokia.cpp \
            qgeoroutereply_nokia.cpp \
            qgeoroutexmlparser.cpp \
            qgeoroutingmanagerengine_nokia.cpp \
            qgeosearchmanagerengine_nokia.cpp \
            qgeosearchreply_nokia.cpp \
            qgeoserviceproviderplugin_nokia.cpp \
            qgeotiledmapdata_nokia.cpp

INCLUDEPATH += $$SOURCE_DIR/src/location \
                $$SOURCE_DIR/src/location/maps \
                $$SOURCE_DIR/src/location/maps/tiled

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002BFCA
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep      
}

RESOURCES += resource.qrc
