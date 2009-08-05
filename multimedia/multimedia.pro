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
    qradiotuner.h \
    qsharedmediaplaylist.h \
    qvideowidget.h \
    qmediastreams.h \
    qmediastreamscontrol.h \
    qaudioencodecontrol.h \
    qvideoencodecontrol.h \
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
    qradiotuner.cpp \
    qsharedmediaplaylist.cpp \
    qvideowidget.cpp \
    qmediastreams.cpp \
    qmediastreamscontrol.cpp \
    qaudioencodecontrol.cpp \
    qvideoencodecontrol.cpp \
    qcamera.cpp \
    qcameracontrol.cpp \
    qcameraservice.cpp \
    qmediaformatcontrol.cpp
include (endpoints/endpoints.pri)
