TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmultimedia_m3u)

PLUGIN_TYPE = playlistformats

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .
INCLUDEPATH += . \
    ../../../src/multimedia

HEADERS += qm3uhandler.h
SOURCES += main.cpp \
           qm3uhandler.cpp
symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    
    #make a sis package from plugin + stub (plugin)
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_SUBDIR}/playlistformats
    DEPLOYMENT += pluginDep
}

target.path=$$QT_MOBILITY_PREFIX/plugins/playlistformats
INSTALLS+=target
