TEMPLATE = subdirs

SUBDIRS = \
        qaudiosource \
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
        qradioplayer \
        qvideowidget

contains(QT_CONFIG, multimedia) {
    SUBDIRS += \
            qgraphicsvideoitem
}
