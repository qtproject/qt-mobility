TEMPLATE = lib
QT += multimedia
CONFIG += plugin
TARGET = QtMobilityMmfEngine
PLUGIN_SUBDIR = mediaservice
include (../../../../common.pri)
qtAddLibrary(QtMedia)

#includes here so that all defines are added here also
include (../common/symbiancommon.pri)
include(mediaplayer/mediaplayer_s60.pri)
include(radio/radio.pri)

# check if we are going to build openmax backend, if yes then disable mmf backend for audiorecording
exists($${EPOCROOT}epoc32/include/platform/mw/khronos/OpenMAXAL.h) {
    message("Not enabling mmf mediarecording backend")
} else {
    message("Enabling mmf mediarecording backend")
    include(audiosource/audiosource_s60.pri)
}



DEPENDPATH += .
INCLUDEPATH += . \
    $${SOURCE_DIR}/include \
    $${SOURCE_DIR}/src/multimedia \
    $${SOURCE_DIR}/src/multimedia/experimental \
    $${SOURCE_DIR}


HEADERS += s60mediaserviceplugin.h

SOURCES += s60mediaserviceplugin.cpp

contains(S60_VERSION, 3.2)|contains(S60_VERSION, 3.1) {
    DEFINES += PRE_S60_50_PLATFORM
}

load(data_caging_paths)
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3=0x2002AC76
TARGET.CAPABILITY = ALL -TCB
MMP_RULES += EXPORTUNFROZEN

#make a sis package from plugin + api + stub (plugin)
pluginDep.sources = $${TARGET}.dll
pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_SUBDIR}
DEPLOYMENT += pluginDep      

#Media API spesific deployment
QtMediaDeployment.sources = QtMedia.dll
QtMediaDeployment.path = /sys/bin

DEPLOYMENT += QtMediaDeployment
