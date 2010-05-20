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
            qgeomappingmanager_nokia_p.h \
            qgeomappingreply_nokia_p.h \
            qgeoplacesmanager_nokia_p.h \
            qgeoplacesreply_nokia_p.h \
            qgeoroutereply_nokia_p.h \
            qgeoroutexmlparser_p.h \
            qgeoroutingmanager_nokia_p.h \
            qgeoserviceproviderplugin_nokia_p.h

SOURCES += \
            qgeocodexmlparser.cpp \
            qgeomappingmanager_nokia.cpp \
            qgeomappingreply_nokia.cpp \
            qgeoplacesmanager_nokia.cpp \
            qgeoplacesreply_nokia.cpp \
            qgeoroutereply_nokia.cpp \
            qgeoroutexmlparser.cpp \
            qgeoroutingmanager_nokia.cpp \
            qgeoserviceproviderplugin_nokia.cpp

INCLUDEPATH += $$SOURCE_DIR/src/location

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep      
}

target.path=$$QT_MOBILITY_PREFIX/plugins/$${PLUGIN_TYPE}
INSTALLS+=target
