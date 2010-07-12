TEMPLATE = subdirs

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData WriteUserData UserEnvironment Location ReadDeviceData TrustedUI
    # multimedia coulld also use MultimediaDD capability
}

include(../../../staticconfig.pri)

contains(mobility_modules,multimedia) {
#Multimedia
    SUBDIRS += \
            qmediaobject_s60 \
            qmediaplayer_s60 \
            qradiotuner_s60

    contains(openmaxal_symbian_enabled, no) {
    SUBDIRS += \
            qmediarecorder_s60
    }

    contains(openmaxal_symbian_enabled, yes) {
    SUBDIRS += \
            qmediarecorder_xa
    }
}
