TEMPLATE = subdirs

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData WriteUserData UserEnvironment Location ReadDeviceData TrustedUI
    # multimedia coulld also use MultimediaDD capability
}

include(../../../staticconfig.pri)

contains(mobility_modules,multimedia) {
#Multimedia
    SUBDIRS += \             
            qcamera_s60 \
            qmediaobject_s60 \
            qmediaplayer_s60 \
            qmediaplaylist_s60 \
            qradiotuner_s60 \
            qmediarecorder_s60
}
