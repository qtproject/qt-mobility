TEMPLATE = lib
CONFIG += plugin
TARGET = gstengine

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

LIBS += -lQtMedia

DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints \
    /usr/include/gstreamer-0.10 \
    /usr/include/glib-2.0 \
    /usr/lib/glib-2.0/include/
QMAKE_CXXFLAGS += $$QT_CFLAGS_GSTREAMER
LIBS += $$QT_LIBS_GSTREAMER \
    -lgstinterfaces-0.10 \
    -lgstvideo-0.10 \
    -lgstbase-0.10 \
    -lgstaudio-0.10 \
    -lasound

# Input
HEADERS += \
    qgstreamermessage.h \
    qgstreamerbushelper.h \
    qgstreamervideowidget.h \
    qgstreamervideorenderer.h \
    qgstreamerserviceplugin.h \
    qalsaaudiodeviceendpoint.h

SOURCES += \
    qgstreamermessage.cpp \
    qgstreamerbushelper.cpp \
    qgstreamervideowidget.cpp \
    qgstreamervideorenderer.cpp \
    qgstreamerserviceplugin.cpp \
    qalsaaudiodeviceendpoint.cpp

multimedia {
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

    LIBS += -lQtMultimedia -lXv
} else {
    DEFINES += QT_NO_VIDEOSURFACE
}

include(mediaplayer/mediaplayer.pri)
include(mediacapture/mediacapture.pri)
