include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs

#ServiceFramework examples
SUBDIRS += filemanagerplugin \
           bluetoothtransferplugin \
           servicebrowser

#BearerManagement examples
SUBDIRS += bearermonitor bearercloud
contains(QT_CONFIG, declarative) {
    SUBDIRS += declarative
}

#Location examples
SUBDIRS += logfilepositionsource
contains(QT_CONFIG, webkit) {
    SUBDIRS += fetchgooglemaps
}

#Contacts examples
SUBDIRS += samplephonebook \
           incomingcalls

#Publish and Subscribe examples
!wince*{
    SUBDIRS += publish-subscribe
    contains(QT_CONFIG, declarative) {
        SUBDIRS += battery-charge
    }
}

#System Information
!maemo:SUBDIRS += sysinfo

#Multimedia - disabled on Symbian due to missing backend
!symbian:SUBDIRS += player \
        cameracapture \
        slideshow \
        streamplayer \
        audiorecorder

#Messaging examples
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo {
    !win32-g++:SUBDIRS += \
        keepintouch\
        querymessages\
        writemessage\
        serviceactions
}
