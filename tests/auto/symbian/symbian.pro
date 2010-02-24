TEMPLATE = subdirs

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData WriteUserData UserEnvironment Location ReadDeviceData TrustedUI
    # multimedia coulld also use MultimediaDD capability
}

include(../../../staticconfig.pri)

contains(mobility_modules,multimedia) {
#Multimedia
    SUBDIRS += \             
            qcamera \
            qmediaobject \
            qmediaplayer \
            qmediaplaylist \
            qradiotuner
}
