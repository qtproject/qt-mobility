TEMPLATE = lib
CONFIG += plugin
TARGET = gstengine
DESTDIR = $$OUT_PWD/../../lib/mediaservice
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
HEADERS += qgstreamerplayercontrol.h \
    qgstreamerplayerservice.h \
    qgstreamerplayersession.h \
    qgstreamermessage.h \
    qgstreamerbushelper.h \
    qgstreamervideowidget.h \
    qgstreamervideorenderer.h \
    qgstreamerserviceplugin.h \
    qgstreamermetadataprovider.h \
    qgstreamerstreamscontrol.h \
    qgstreamercaptureservice.h \
    qalsaaudiodeviceendpoint.h \
    qgstreamercapturesession.h

SOURCES += qgstreamerplayercontrol.cpp \
    qgstreamerplayerservice.cpp \
    qgstreamerplayersession.cpp \
    qgstreamermessage.cpp \
    qgstreamerbushelper.cpp \
    qgstreamervideowidget.cpp \
    qgstreamervideorenderer.cpp \
    qgstreamerserviceplugin.cpp \
    qgstreamermetadataprovider.cpp \
    qgstreamerstreamscontrol.cpp \
    qgstreamercaptureservice.cpp \
    qalsaaudiodeviceendpoint.cpp \
    qgstreamercapturesession.cpp


multimedia {
    SOURCES += \
        qvideosurfacegstsink.cpp \
        qgstvideobuffer.cpp

    HEADERS += \
        qvideosurfacegstsink.h \
        qgstvideobuffer.h

    LIBS += -lQtMultimedia
} else {
    DEFINES += QT_NO_VIDEOSURFACE
}
