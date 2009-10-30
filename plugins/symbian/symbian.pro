TEMPLATE = lib
CONFIG += plugin
TARGET = QtMobilityMultimediaEngine
PLUGIN_SUBDIR = mediaservice
include (../../common.pri)
LIBS += -lQtMedia -lcamerawrapper -lfbscli

DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia
HEADERS += s60serviceplugin.h \
           s60videowidget.h
   
SOURCES += s60serviceplugin.cpp \
           s60videowidget.cpp

include(radio/radio.pri)
include(camera/camera_s60.pri)
include(mediaplayer/mediaplayer_s60.pri)
include(audiosource/audiosource_s60.pri)

load(data_caging_paths)
pluginDep.sources = $${TARGET}.dll
pluginDep.path = $$QT_PLUGINS_BASE_DIR/$$PLUGIN_SUBDIR
DEPLOYMENT += pluginDep
TARGET.EPOCALLOWDLLDATA = 1
