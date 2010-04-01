TEMPLATE = subdirs

include(../../staticconfig.pri)

contains(mobility_modules,serviceframework) {
    SUBDIRS += databasemanager \                #service framework
           servicemetadata \
           qserviceinterfacedescriptor \
           qservicefilter \
           qservicemanager \
           qabstractsecuritysession \
           qservicecontext \
           icheck

# servicedatabase is not compiled into the serviceframework library on symbian,
# special handling is needed
    !symbian:SUBDIRS+=servicedatabase
}

contains(mobility_modules,bearer) {
    SUBDIRS += qnetworkconfigmanager \          #Bearer management
           qnetworkconfiguration \
           qnetworksession
}

contains(mobility_modules,location) {
    SUBDIRS += qgeocoordinate \                 #Location
          qgeopositioninfo \
          qgeosatelliteinfo \
          qgeosatelliteinfosource \
          qgeopositioninfosource \
          qgeoareamonitor \
          qlocationutils \
          qnmeapositioninfosource

    wince* {
        SUBDIRS += qgeoinfosources_wince
    }
}


contains(mobility_modules,publishsubscribe) {
    SUBDIRS += qvaluespace \                           #Publish and Subscribe
           qvaluespacepublisher \
           qvaluespacesubscriber \
           qcrmlparser

    unix|win32 {
        !symbian:!maemo6:!maemo5: SUBDIRS+= \
            qsystemreadwritelock \
            qsystemreadwritelock_oop
    }

    unix:!symbian:!maemo6:!maemo5 {
        SUBDIRS+= \
               qpacket \
               qmallocpool \
               qpacketprotocol
    }
}

contains(mobility_modules,systeminfo) {
    SUBDIRS += qsysteminfo \                    #SystemInformation
          qsystemdeviceinfo \
          qsystemdisplayinfo \
          qsystemstorageinfo \
          qsystemnetworkinfo \
          qsystemscreensaver
}

contains(mobility_modules,contacts) {
    #Contacts
    SUBDIRS +=  qcontact \
            qcontactactions \
            qcontactasync \
            qcontactdetail \
            qcontactdetaildefinition \
            qcontactdetails \
            qcontactfilter \
            qcontactmanager \
            qcontactmanagerplugins \
            qcontactmanagerfiltering \
            qcontactrelationship \
            qlatin1constant
}

contains(mobility_modules,versit) {
    # Versit module
    SUBDIRS += \
            qvcard21writer \
            qvcard30writer \
            qversit \
            qversitcontactexporter \
            qversitcontactimporter \
            qversitdocument \
            qversitproperty \
            qversitreader \
            qversitwriter
}

contains(mobility_modules,multimedia) {
    SUBDIRS += \             #Multimedia
        qaudiocapturesource \
        qgraphicsvideoitem \
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
        qradiotuner \
        qpaintervideosurface \
        qvideowidget \
        qmediatimerange

    symbian: {
        #symbian spesific autotests
        SUBDIRS += symbian 
        SUBDIRS -= \
                qmediaplayer_s60 \
                qradiotuner_s60 \
                qmediaobject_s60 \
                qmediarecorder_s60
    }
}
#Messaging
contains(mobility_modules,messaging) {
    contains(qmf_enabled,yes)|wince*|win32|symbian|maemo5 {
    !win32-g++:SUBDIRS += \
        qmessagestore \
        qmessagestorekeys \
        qmessage \
        qmessageservice
    }
}
