TEMPLATE = lib
CONFIG += plugin
TARGET = gstengine

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

LIBS += -lQtMedia

DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

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
    qgstreamervideowidget.h \
    qgstreamervideorenderer.h \
    qgstreamerserviceplugin.h \
    qgstreameraudioinputdevicecontrol.h \
    qgstaudiodeviceendpoint.h

SOURCES += \
    qgstreamermessage.cpp \
    qgstreamerbushelper.cpp \
    qgstreamervideooutputcontrol.cpp \
    qgstreamervideowidget.cpp \
    qgstreamervideorenderer.cpp \
    qgstreamerserviceplugin.cpp \
    qgstreameraudioinputdevicecontrol.cpp \
    qgstaudiodeviceendpoint.cpp

videosurface {
    QT += multimedia

    SOURCES += \
        qgstreamervideooverlay.cpp \
        qgstvideobuffer.cpp \
        qvideosurfacegstsink.cpp \
        qx11videosurface.cpp

    HEADERS += \
        qgstreamervideooverlay.h \
        qgstvideobuffer.h \
        qvideosurfacegstsink.h \
        qx11videosurface.h

    LIBS += -lXv
}

include(mediaplayer/mediaplayer.pri)
include(mediacapture/mediacapture.pri)
