TEMPLATE = subdirs

include($$QT_MOBILITY_BUILD_TREE/config.pri)

contains(mobility_modules,multimedia) {
#Multimedia
    SUBDIRS += \             
            qcamera \
            qmediaobject \
            qmediaplayer \
            qmediaplaylist \
            qradiotuner
}