TEMPLATE = lib
QT += multimedia
CONFIG += plugin
TARGET = QtMobilityMultimediaEngine
PLUGIN_SUBDIR = mediaservice
include (../../../common.pri)

qtAddLibrary(QtMedia)

DEPENDPATH += .
INCLUDEPATH += . \
    $${SOURCE_DIR}/include \
    $${SOURCE_DIR}/src/multimedia \
    $${SOURCE_DIR}/src/multimedia/experimental \
    $${SOURCE_DIR}


HEADERS += s60mediaserviceplugin.h \
           s60videooutputcontrol.h
           
SOURCES += s60mediaserviceplugin.cpp \
           s60videooutputcontrol.cpp 

include(camera/camera_s60.pri)
include(radio/radio.pri)

# WINSCW compile depends on mpengine.lib, found in S60 SDK greater than 3.1
include(mediaplayer/mediaplayer_s60.pri)

include(audiosource/audiosource_s60.pri)

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

#Dependency definitions to pkg file
qtmobilitymultimedia.pkg_prerules += "; Dependency to camerawrapper"
qtmobilitymultimedia.pkg_prerules += "(0x2001EC5F), 1, 20, 0, {\"CameraWrapper\"}"
qtmobilitymultimedia.pkg_prerules += "; Dependency to Qt Multimedia"
qtmobilitymultimedia.pkg_prerules += "(0x2001E627), 4, 6, 0, {\"QtMultimedia\"}"

DEPLOYMENT += QtMediaDeployment qtmobilitymultimedia
