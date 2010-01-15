TEMPLATE = subdirs

symbian {
        TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData WriteUserData UserEnvironment Location ReadDeviceData TrustedUI
        # multimedia coulld also use MultimediaDD capability
}

include($$QT_MOBILITY_BUILD_TREE/config.pri)

contains(mobility_modules,multimedia) {
#Multimedia
    SUBDIRS += \             
            qcamera \
            qmediaplayer \
            qradiotuner
}