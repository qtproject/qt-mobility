TEMPLATE = lib

# distinct from QtMultimedia
TARGET = QtMedia

include (../common.pri)

contains(QT_CONFIG, multimedia): QT += multimedia

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_MEDIA_LIB

HEADERS = \
    qabstractmediacontrol.h \
    qabstractmediacontrol_p.h \
    qabstractmediaobject.h \
    qabstractmediaobject_p.h \
    qabstractmediaservice.h \
    qlocalmediaplaylistsource.h \
    qmediaimageviewer.h \
    qmediametadata.h \
    qmetadataprovidercontrol.h \
    qmediaplayer.h \
    qmediaplayercontrol.h \
    qmediaplayerservice.h \
    qmediaplaylist.h \
    qmediaplaylist_p.h \
    qmediaplaylistnavigator.h \
    qmediaplaylistmodel.h \
    qmediaplaylistsource.h \
    qmediaplaylistioplugin.h \
    qmediaproviderfactory_p.h \
    qmediaresource.h \
    qmediarecorder.h \
    qmediarecordercontrol.h \
    qmediaserviceprovider.h \
    qmediaserviceproviderplugin.h \
    qmediasink.h \
    qmultimediaglobal.h \
    qradioplayer.h \
    qradioservice.h \
    qradioplayercontrol.h \
    qsharedmediaplaylist.h \
    qvideooutputcontrol.h \
    qvideowidget.h \
    qvideowindowcontrol.h \
    qmediastreams.h \
    qmediastreamscontrol.h \
    qaudioencodecontrol.h \
    qvideoencodecontrol.h \
    qimageencodecontrol.h \
    qcamera.h \
    qcameracontrol.h \
    qcameraservice.h \
    qmediaformatcontrol.h \
    qmediaplaylistcontrol.h \
    qaudioinputdevicecontrol.h

SOURCES = qabstractmediacontrol.cpp \
    qabstractmediaobject.cpp \
    qabstractmediaservice.cpp \
    qlocalmediaplaylistsource.cpp \
    qmediaimageviewer.cpp \
    qmediametadata.cpp \
    qmetadataprovidercontrol.cpp \
    qmediaplayer.cpp \
    qmediaplayercontrol.cpp \
    qmediaplayerservice.cpp \
    qmediaplaylist.cpp \
    qmediaplaylistioplugin.cpp \
    qmediaplaylistnavigator.cpp \
    qmediaplaylistmodel.cpp \
    qmediaplaylistsource.cpp \
    qmediaproviderfactory.cpp \
    qmediarecorder.cpp \
    qmediarecordercontrol.cpp \
    qmediaresource.cpp \
    qmediaserviceprovider.cpp \
    qmediasink.cpp \
    qradioplayer.cpp \
    qradioservice.cpp \
    qradioplayercontrol.cpp \
    qsharedmediaplaylist.cpp \
    qvideooutputcontrol.cpp \
    qvideowidget.cpp \
    qvideowindowcontrol.cpp \
    qmediastreams.cpp \
    qmediastreamscontrol.cpp \
    qaudioencodecontrol.cpp \
    qvideoencodecontrol.cpp \
    qimageencodecontrol.cpp \
    qcamera.cpp \
    qcameracontrol.cpp \
    qcameraservice.cpp \
    qmediaformatcontrol.cpp \
    qmediaplaylistcontrol.cpp \
    qaudioinputdevicecontrol.cpp

videosurface {
    HEADERS += \
        qpaintervideosurface_p.h \
        qvideorenderercontrol.h

    SOURCES += \
        qpaintervideosurface.cpp \
        qvideorenderercontrol.cpp
}

include (endpoints/endpoints.pri)
