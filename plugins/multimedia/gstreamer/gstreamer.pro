TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmedia_gstengine)
PLUGIN_TYPE=mediaservice

include(../../../common.pri)
INCLUDEPATH+=../../../src/multimedia

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

unix:contains(QT_CONFIG, alsa) {
DEFINES += HAVE_ALSA
LIBS += \
    -lasound
}

CONFIG += link_pkgconfig

PKGCONFIG += \
    gstreamer-0.10 \
    gstreamer-base-0.10 \
    gstreamer-interfaces-0.10 \
    gstreamer-audio-0.10 \
    gstreamer-video-0.10

maemo* {
  PKGCONFIG +=gstreamer-plugins-bad-0.10
}

# Input
HEADERS += \
    qgstreamermessage.h \
    qgstreamerbushelper.h \
    qgstreamervideooutputcontrol.h \
    qgstreamervideorendererinterface.h \
    qgstreamervideowidget.h \
    qgstreamerserviceplugin.h \
    qgstreameraudioinputendpointselector.h \
    qgstreamervideoinputdevicecontrol.h

SOURCES += \
    qgstreamermessage.cpp \
    qgstreamerbushelper.cpp \
    qgstreamervideooutputcontrol.cpp \
    qgstreamervideorendererinterface.cpp \
    qgstreamervideowidget.cpp \
    qgstreamerserviceplugin.cpp \
    qgstreameraudioinputendpointselector.cpp \
    qgstreamervideoinputdevicecontrol.cpp

contains(QT_CONFIG, multimedia) {
    QT += multimedia

    SOURCES += \
        qgstreamervideooverlay.cpp \
        qgstreamervideorenderer.cpp \
        qgstvideobuffer.cpp \
        qvideosurfacegstsink.cpp \
        qx11videosurface.cpp \
        qgstxvimagebuffer.cpp

    HEADERS += \
        qgstreamervideooverlay.h \
        qgstreamervideorenderer.h \
        qgstvideobuffer.h \
        qvideosurfacegstsink.h \
        qx11videosurface.h \
        qgstxvimagebuffer.h


    LIBS += -lXv
}

include(mediaplayer/mediaplayer.pri)
!maemo* {
    include(mediacapture/mediacapture.pri)
} else {
    include(mediacapture/maemo/mediacapture_maemo.pri)
    DEFINES += GST_USE_UNSTABLE_API #prevents warnings because of unstable photography API 
}

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target
