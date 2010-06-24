TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtgeoservices_nokia)
PLUGIN_TYPE=geoservices

include(../../../common.pri)

QT += network

CONFIG += mobility
MOBILITY = location

HEADERS += \
            qgeocodexmlparser_p.h \
            qgeomappingmanagerengine_nokia_p.h \
            qgeomapreply_nokia_p.h \
            #qgeomapviewport_nokia_p.h \
            qgeoroutereply_nokia_p.h \
            qgeoroutexmlparser_p.h \
            qgeoroutingmanagerengine_nokia_p.h \
            qgeosearchmanagerengine_nokia_p.h \
            qgeosearchreply_nokia_p.h \
            qgeoserviceproviderplugin_nokia_p.h

SOURCES += \
            qgeocodexmlparser.cpp \
            qgeomappingmanagerengine_nokia.cpp \
            qgeomapreply_nokia.cpp \
            #qgeomapviewport_nokia.cpp \
            qgeoroutereply_nokia.cpp \
            qgeoroutexmlparser.cpp \
            qgeoroutingmanagerengine_nokia.cpp \
            qgeosearchmanagerengine_nokia.cpp \
            qgeosearchreply_nokia.cpp \
            qgeoserviceproviderplugin_nokia.cpp

INCLUDEPATH += $$QT_MOBILITY_BUILD_TREE/include/QtmLocation
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/src/location

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002BFCA
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep      
}

target.path=$$QT_MOBILITY_PREFIX/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
