TEMPLATE = subdirs

SUBDIRS = \
        qaudiocapturesource \
        qcamera \
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
        qvideowidget

contains(QT_CONFIG, multimedia) {
    SUBDIRS += \
            qgraphicsvideoitem \
            qpaintervideosurface

}

symbian { 
SUBDIRS -= \
        qaudiocapturesource \
        qmediaplaylistnavigator \
        qmediaimageviewer \
        qmediapluginloader \
        qmediaserviceprovider \
        qpaintervideosurface \
        qvideowidget \
        qmediaplayer \
        qgraphicsvideoitem \
        qmediaplaylist
}

