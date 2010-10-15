include(../../staticconfig.pri)

requires(contains(mobility_modules,multimedia))

TEMPLATE = subdirs
SUBDIRS += \
    qabstractvideobuffer \
    qabstractvideosurface \
    qaudiocapturesource \
    qgraphicsvideoitem \
    qmediaimageviewer \
    qmediaobject \
    qmediaplayer \
    qmediaplaylist \
    qmediaplaylistnavigator \
    qmediapluginloader \
    qmediarecorder \
    qmediaresource \
    qmediaservice \
    qmediaserviceprovider \
    qmediacontent \
    qradiotuner \
    qcamera \
    qpaintervideosurface \
    qvideowidget \
    qmediatimerange \
    qaudiodeviceinfo \
    qaudiooutput \
    qaudioinput \
    qaudioformat \
    qvideoframe \
    qvideosurfaceformat

contains (QT_CONFIG, declarative) {
    SUBDIRS += \
        qsoundeffect \
        qdeclarativeaudio \
        qdeclarativevideo
}

