include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs

SUBDIRS += hapticsplayer

#ServiceFramework examples
contains(mobility_modules,serviceframework) {
    SUBDIRS += filemanagerplugin \
            bluetoothtransferplugin \
            servicebrowser
}

#BearerManagement examples
contains(mobility_modules,bearer) {
    SUBDIRS += bearermonitor bearercloud
    contains(QT_CONFIG, declarative) {
        SUBDIRS += declarative
    }
}

#Location examples
contains(mobility_modules,location) {
    SUBDIRS += logfilepositionsource
    contains(QT_CONFIG, webkit) {
        SUBDIRS += fetchgooglemaps
    }
}

#Contacts examples
contains(mobility_modules,contacts) {
    SUBDIRS += samplephonebook \
            incomingcalls
}

#Publish and Subscribe examples
contains(mobility_modules,publishsubscribe) {
    !wince*{
        SUBDIRS += publish-subscribe
        contains(QT_CONFIG, declarative) {
            SUBDIRS += battery-charge
        }
    }
}

#System Information
contains(mobility_modules,systeminfo): SUBDIRS += sysinfo

#Multimedia
contains(mobility_modules,multimedia) {
    #disabled on Symbian due to missing backend
    !symbian:SUBDIRS += \
        radio \
        player \
        cameracapture \
        slideshow \
        streamplayer \
        audiorecorder
}

#Messaging examples
contains(mobility_modules,messaging) {
    contains(qmf_enabled,yes)|wince*|win32|symbian|maemo6 {
    !win32-g++:SUBDIRS += \
        keepintouch\
        querymessages\
        writemessage\
        serviceactions
    }
}
