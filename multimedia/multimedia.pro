TEMPLATE = lib

# QT += multimedia
# DEFINES += AUDIOSERVICES
# DEFINES += VIDEOSERVICES
# distinct from QtMultimedia
TARGET = QtMedia

include (../common.pri)

DLLDESTDIR = $$DESTDIR

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_MEDIA_LIB
HEADERS = qabstractmediacontrol.h \
    qabstractmediaobject.h \
    qabstractmediaobject_p.h \
    qabstractmediaservice.h \
    qlocalmediaplaylistsource.h \
    qmediaimageviewer.h \
    qmediametadata.h \
    qmetadataprovider.h \
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
    qmediarecorderservice.h \
    qmediaserviceprovider.h \
    qmediaserviceproviderplugin.h \
    qmediasink.h \
    qmultimediaglobal.h \
    qradioplayer.h \
    qradioservice.h \
    qradiotuner.h \
    qsharedmediaplaylist.h \
    qvideowidget.h \
    qaudiocapturecontrol.h \
    qaudiocaptureservice.h \
    qaudiocapture.h \
    qmediastreams.h \
    qmediastreamscontrol.h \
    qmediacapturecontrol.h \
    qaudioencodecontrol.h \
    qvideoencodecontrol.h \
    qmediacapture.h \
    qcamera.h \
    qcameracontrol.h \
    qcameraservice.h \
    qmediaformatcontrol.h
SOURCES = qabstractmediacontrol.cpp \
    qabstractmediaobject.cpp \
    qabstractmediaservice.cpp \
    qlocalmediaplaylistsource.cpp \
    qmediaimageviewer.cpp \
    qmediametadata.cpp \
    qmetadataprovider.cpp \
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
    qmediarecorderservice.cpp \
    qmediaresource.cpp \
    qmediaserviceprovider.cpp \
    qmediasink.cpp \
    qradioplayer.cpp \
    qradioservice.cpp \
    qradiotuner.cpp \
    qsharedmediaplaylist.cpp \
    qvideowidget.cpp \
    qaudiocapturecontrol.cpp \
    qaudiocaptureservice.cpp \
    qaudiocapture.cpp \
    qmediastreams.cpp \
    qmediastreamscontrol.cpp \
    qmediacapturecontrol.cpp \
    qaudioencodecontrol.cpp \
    qvideoencodecontrol.cpp \
    qmediacapture.cpp \
    qcamera.cpp \
    qcameracontrol.cpp \
    qcameraservice.cpp \
    qmediaformatcontrol.cpp
include (endpoints/endpoints.pri)
