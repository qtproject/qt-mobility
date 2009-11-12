TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(gstengine)
PLUGIN_TYPE=mediaservice

include(../../common.pri)
INCLUDEPATH+=../../src/multimedia

qtAddLibrary(QtMedia)

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

# Input
HEADERS += \
    qgstreamermessage.h \
    qgstreamerbushelper.h \
    qgstreamervideooutputcontrol.h \
    qgstreamervideorendererinterface.h \
    qgstreamervideowidget.h \
    qgstreamerserviceplugin.h \
    qgstreameraudioinputdevicecontrol.h \
    qgstreamervideoinputdevicecontrol.h

SOURCES += \
    qgstreamermessage.cpp \
    qgstreamerbushelper.cpp \
    qgstreamervideooutputcontrol.cpp \
    qgstreamervideorendererinterface.cpp \
    qgstreamervideowidget.cpp \
    qgstreamerserviceplugin.cpp \
    qgstreameraudioinputdevicecontrol.cpp \
    qgstreamervideoinputdevicecontrol.cpp

contains(QT_CONFIG, multimedia) {
    QT += multimedia

    SOURCES += \
        qgstreamervideooverlay.cpp \
        qgstreamervideorenderer.cpp \
        qgstvideobuffer.cpp \
        qvideosurfacegstsink.cpp \
        qx11videosurface.cpp

    HEADERS += \
        qgstreamervideooverlay.h \
        qgstreamervideorenderer.h \
        qgstvideobuffer.h \
        qvideosurfacegstsink.h \
        qx11videosurface.h

    LIBS += -lXv
}

include(mediaplayer/mediaplayer.pri)
include(mediacapture/mediacapture.pri)

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target
