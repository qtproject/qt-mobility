TEMPLATE = lib
QT += multimedia
CONFIG += plugin
TARGET = QtMobilityMultimediaEngine
PLUGIN_SUBDIR = mediaservice
include (../../../common.pri)
LIBS += -lQtMedia

# camera is on hold -lcamerawrapper -lfbscli

DEPENDPATH += .
INCLUDEPATH += . \
    $${SOURCE_DIR}/include \
    $${SOURCE_DIR}/src/multimedia


HEADERS += s60serviceplugin.h \
           s60videowidget.h \
           s60videooutputcontrol.h
           
SOURCES += s60serviceplugin.cpp \
           s60videowidget.cpp \
           s60videooutputcontrol.cpp

include(radio/radio.pri)
# include(camera/camera_s60.pri)

# depends on mpengine.lib, found in S60 SDK greater than 3.1
exists($${EPOCROOT}epoc32/release/winscw/udeb/mpengine.lib){
    include(mediaplayer/mediaplayer_s60.pri)
    DEFINES += HAS_MEDIA_PLAYER
}
include(audiosource/audiosource_s60.pri)

load(data_caging_paths)
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3=0x2002AC76
MMP_RULES += EXPORTUNFROZEN

#make a sis package from plugin + api + stub (plugin)
pluginDep.sources = $${TARGET}.dll
pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_SUBDIR}
DEPLOYMENT += pluginDep      

#Media API spesific deployment
QtMediaDeployment.sources = QtMedia.dll
QtMediaDeployment.path = /sys/bin
DEPLOYMENT += QtMediaDeployment
