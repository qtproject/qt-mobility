include(../common.pri)

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
SUBDIRS += sysinfo

#Multimedia
SUBDIRS += player \
        cameracapture \
        slideshow \
        streamplayer \
        audiorecorder

#Messaging examples
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo {
    SUBDIRS += \
        keepintouch\
        querymessages\
        sendmessage\
        serviceactions
}
