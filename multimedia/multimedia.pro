TEMPLATE = lib

# distinct from QtMultimedia
TARGET = QtMedia

include (../common.pri)

QT += network

contains(QT_CONFIG, opengl): QT += opengl

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_MEDIA_LIB

HEADERS = \
    qabstractmediacontrol.h \
    qabstractmediacontrol_p.h \
    qabstractmediaobject.h \
    qabstractmediaobject_p.h \
    qabstractmediaservice.h \
    qabstractmediaservice_p.h  \
    qaudiorecorderservice.h \
    qlocalmediaplaylistprovider.h \
    qmediametadata.h \
    qmetadataprovidercontrol.h \
    qmediaplayer.h \
    qmediaplayercontrol.h \
    qmediaplayerservice.h \
    qmediaplaylist.h \
    qmediaplaylist_p.h \
    qmediaplaylistnavigator.h \
    qmediaplaylistprovider.h \
    qmediaplaylistprovider_p.h \
    qmediaplaylistioplugin.h \
    qmediaproviderfactory_p.h \
    qmediasource.h \
    qmediaresource.h \
    qmediarecorder.h \
    qmediarecorderservice.h \
    qmediarecordercontrol.h \
    qmediaserviceprovider.h \
    qmediaserviceproviderplugin.h \
    qmediaslideshow.h \
    qmediaslideshowservice_p.h \
    qmultimediaglobal.h \
    qradioplayer.h \
    qradioservice.h \
    qradioplayercontrol.h \
    qvideooutputcontrol.h \
    qvideowidget.h \
    qvideowidget_p.h \
    qvideowindowcontrol.h \
    qvideowidgetcontrol.h \
    qaudioencodercontrol.h \
    qvideoencodercontrol.h \
    qimageencodercontrol.h \
    qcamera.h \
    qcameracontrol.h \
    qcameraexposurecontrol.h \
    qcamerafocuscontrol.h \
    qcameraservice.h \
    qmediaformatcontrol.h \
    qmediaplaylistcontrol.h \
    qaudiodevicecontrol.h \
    qvideodevicecontrol.h \
    qmediapluginloader_p.h

SOURCES = qabstractmediacontrol.cpp \
    qabstractmediaobject.cpp \
    qabstractmediaservice.cpp \
    qaudiorecorderservice.cpp \
    qlocalmediaplaylistprovider.cpp \
    qmediametadata.cpp \
    qmetadataprovidercontrol.cpp \
    qmediaplayer.cpp \
    qmediaplayercontrol.cpp \
    qmediaplayerservice.cpp \
    qmediaplaylist.cpp \
    qmediaplaylistioplugin.cpp \
    qmediaplaylistnavigator.cpp \
    qmediaplaylistprovider.cpp \
    qmediaproviderfactory.cpp \
    qmediarecorder.cpp \
    qmediarecorderservice.cpp \
    qmediarecordercontrol.cpp \
    qmediasource.cpp \
    qmediaresource.cpp \
    qmediaserviceprovider.cpp \
    qmediaslideshow.cpp \
    qmediaslideshowservice.cpp \
    qradioplayer.cpp \
    qradioservice.cpp \
    qradioplayercontrol.cpp \
    qvideooutputcontrol.cpp \
    qvideowidget.cpp \
    qvideowindowcontrol.cpp \
    qvideowidgetcontrol.cpp \
    qaudioencodercontrol.cpp \
    qvideoencodercontrol.cpp \
    qimageencodercontrol.cpp \
    qcamera.cpp \
    qcameracontrol.cpp \
    qcameraexposurecontrol.cpp \
    qcamerafocuscontrol.cpp \
    qcameraservice.cpp \
    qmediaformatcontrol.cpp \
    qmediaplaylistcontrol.cpp \
    qaudiodevicecontrol.cpp \
    qvideodevicecontrol.cpp \
    qmediapluginloader.cpp

contains(QT_CONFIG, multimedia) {
    HEADERS += \
        qpaintervideosurface_p.h \
        qvideorenderercontrol.h

    SOURCES += \
        qpaintervideosurface.cpp \
        qvideorenderercontrol.cpp
} else {
    HEADERS += qaudioformat.h
    SOURCES += qaudioformat.cpp
}
