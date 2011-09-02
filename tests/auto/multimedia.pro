include(../../staticconfig.pri)

requires(contains(mobility_modules,multimedia))

TEMPLATE = subdirs
SUBDIRS += \
    qabstractvideobuffer \
    qabstractvideosurface \
    qaudiocapturesource \
    qaudiodeviceinfo \
    qaudioformat \
    qaudioinput \
    qaudiooutput \
    qcamera \
    qcamerabackend \
    qcameraimagecapture \
    qcameraviewfinder \
    qgraphicsvideoitem \
    qmediabindableinterface \
    qmediacontainercontrol \
    qmediacontent \
    qmediaimageviewer \
    qmediaobject \
    qmediaplayer \
    qmediaplayerbackend \
    qmediaplaylist \
    qmediaplaylistnavigator \
    qmediapluginloader \
    qmediarecorder \
    qmediaresource \
    qmediaservice \
    qmediaserviceprovider \
    qmediatimerange \
    qpaintervideosurface \
    qradiotuner \
    qvideoframe \
    qvideosurfaceformat \
    qvideowidget \
    qmetadatareadercontrol \
    qmetadatawritercontrol \

# This is disabled because it is unfinished
# qmediastreamscontrol \

# These is disabled until intent is clearer
#    qvideodevicecontrol \
#    qvideoencodercontrol \

# This is a commment for the mock backend directory so that maketestselftest
# doesn't believe it's an untested directory
# qmultimedia_common


contains (QT_CONFIG, declarative) {
    SUBDIRS += \
        qsoundeffect \
        qdeclarativeaudio \
        qdeclarativevideo
}
