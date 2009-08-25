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
#    qsharedmediaplaylist.h \
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
    qcameraexposurecontrol.h \
    qcamerafocuscontrol.h \
    qcameraservice.h \
    qmediaformatcontrol.h \
    qmediaplaylistcontrol.h \
    qaudioinputdevicecontrol.h \
    qmediapluginloader_p.h

SOURCES = qabstractmediacontrol.cpp \
    qabstractmediaobject.cpp \
    qabstractmediaservice.cpp \
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
    qmediarecordercontrol.cpp \
    qmediaresource.cpp \
    qmediaserviceprovider.cpp \
    qmediasink.cpp \
    qradioplayer.cpp \
    qradioservice.cpp \
    qradioplayercontrol.cpp \
#    qsharedmediaplaylist.cpp \
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
    qcameraexposurecontrol.cpp \
    qcamerafocuscontrol.cpp \
    qcameraservice.cpp \
    qmediaformatcontrol.cpp \
    qmediaplaylistcontrol.cpp \
    qaudioinputdevicecontrol.cpp \
    qmediapluginloader.cpp

videosurface {
    HEADERS += \
        qpaintervideosurface_p.h \
        qvideorenderercontrol.h

    SOURCES += \
        qpaintervideosurface.cpp \
        qvideorenderercontrol.cpp
}

include (endpoints/endpoints.pri)
