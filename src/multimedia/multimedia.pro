TEMPLATE = lib

# distinct from QtMultimedia
TARGET = QtMultimedia

include (../../common.pri)
INCLUDEPATH+= .

QT += network

contains(QT_CONFIG, opengl): QT += opengl

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_MULTIMEDIA_LIB

PRIVATE_HEADERS += \
    qmediacontrol_p.h \
    qmediaobject_p.h \
    qmediaservice_p.h  \
    qmediaplaylist_p.h \
    qmediaplaylistprovider_p.h \
    qmediaimageviewerservice_p.h \
    qvideowidget_p.h \
    qmediapluginloader_p.h \
    qpaintervideosurface_p.h

PUBLIC_HEADERS += \
    qmediacontrol.h \
    qmediaobject.h \
    qmediaservice.h \
    qmediabindableinterface.h \
    qlocalmediaplaylistprovider.h \
    qmediaimageviewer.h \
    qmediaplayer.h \
    qmediaplayercontrol.h \
    qmediaplaylist.h \
    qmediaplaylistnavigator.h \
    qmediaplaylistprovider.h \
    qmediaplaylistioplugin.h \
    qmediacontent.h \
    qmediaresource.h \
    qmediarecorder.h \
    qmediaencodersettings.h \
    qmediarecordercontrol.h \
    qmediaserviceprovider.h \
    qmediaserviceproviderplugin.h \
    qmetadatareadercontrol.h \
    qmetadatawritercontrol.h \
    qmediastreamscontrol.h \
    qradiotuner.h \
    qradiotunercontrol.h \
    qtmedianamespace.h \
    qvideowidget.h \
    qvideowindowcontrol.h \
    qvideowidgetcontrol.h \
    qaudioencodercontrol.h \
    qvideoencodercontrol.h \
    qimageencodercontrol.h \
    qaudiocapturesource.h \
    qmediacontainercontrol.h \
    qmediaplaylistcontrol.h \
    qmediaplaylistsourcecontrol.h \
    qaudioendpointselector.h \
    qvideodevicecontrol.h \
    qgraphicsvideoitem.h \
    qvideorenderercontrol.h \
    qmediatimerange.h

SOURCES += qmediacontrol.cpp \
    qmediaobject.cpp \
    qmediaservice.cpp \
    qmediabindableinterface.cpp \
    qlocalmediaplaylistprovider.cpp \
    qmediaimageviewer.cpp \
    qmediaimageviewerservice.cpp \
    qmediaplayer.cpp \
    qmediaplayercontrol.cpp \
    qmediaplaylist.cpp \
    qmediaplaylistioplugin.cpp \
    qmediaplaylistnavigator.cpp \
    qmediaplaylistprovider.cpp \
    qmediarecorder.cpp \
    qmediaencodersettings.cpp \
    qmediarecordercontrol.cpp \
    qmediacontent.cpp \
    qmediaresource.cpp \
    qmediaserviceprovider.cpp \
    qmetadatareadercontrol.cpp \
    qmetadatawritercontrol.cpp \
    qmediastreamscontrol.cpp \
    qradiotuner.cpp \
    qradiotunercontrol.cpp \
    qvideowidget.cpp \
    qvideowindowcontrol.cpp \
    qvideowidgetcontrol.cpp \
    qaudioencodercontrol.cpp \
    qvideoencodercontrol.cpp \
    qimageencodercontrol.cpp \
    qaudiocapturesource.cpp \
    qmediacontainercontrol.cpp \
    qmediaplaylistcontrol.cpp \
    qmediaplaylistsourcecontrol.cpp \
    qaudioendpointselector.cpp \
    qvideodevicecontrol.cpp \
    qmediapluginloader.cpp \
    qpaintervideosurface.cpp \
    qvideorenderercontrol.cpp \
    qmediatimerange.cpp

include(audio/audio.pri)
include(video/video.pri)
include(effects/effects.pri)

mac {
   HEADERS += qpaintervideosurface_mac_p.h
   OBJECTIVE_SOURCES += qpaintervideosurface_mac.mm

   LIBS += -framework AppKit -framework QuartzCore -framework QTKit
}

maemo5 {
    QMAKE_CXXFLAGS += -march=armv7a -mcpu=cortex-a8 -mfloat-abi=softfp -mfpu=neon
    HEADERS += qxvideosurface_maemo5_p.h
    SOURCES += qxvideosurface_maemo5.cpp
    SOURCES += qgraphicsvideoitem_maemo5.cpp
    LIBS += -lXv
} else {
    SOURCES += qgraphicsvideoitem.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

symbian {
    load(data_caging_paths)
    QtMediaDeployment.sources = QtMultimedia.dll
    QtMediaDeployment.path = /sys/bin
    DEPLOYMENT += QtMediaDeployment
    TARGET.UID3=0x2001E627
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lefsrv
}

CONFIG += middleware
include(../../features/deploy.pri)
