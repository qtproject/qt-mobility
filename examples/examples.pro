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
SUBDIRS += publish-subscribe
contains(QT_CONFIG, declarative) {
    SUBDIRS += battery-charge
}

#System Information
SUBDIRS += sysinfo

#Multimedia
SUBDIRS += player \
        recorder \
        cameracapture \
        slideshow \
        streamplayer

#Messaging examples
SUBDIRS += \
    keepintouch\
    querymessages\
    sendmessage\
    serviceactions

