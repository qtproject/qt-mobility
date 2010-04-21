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
          qgeoboundingbox \
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

    SUBDIRS += qlandmarkmanagerengine_sqlite
}

contains(mobility_modules,landmarks) {
    SUBDIRS += qlandmark \                      #Landmark
            qlandmarkcategory
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
    SUBDIRS +=  qcontact \                      #Contacts
            qcontactactions \
            qcontactasync \
            qcontactdetail \
            qcontactdetaildefinition \
            qcontactdetails \
            qcontactfilter \
            qcontactmanager \
            qcontactmanagerplugins \
            qcontactmanagerfiltering \
            qcontactrelationship
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
            qversitutils \
            qversitwriter
}

contains(mobility_modules,multimedia) {
    SUBDIRS += \             #Multimedia
        qaudiocapturesource \
        qcamera \
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
        qvideowidget \
        qmediatimerange

    contains(QT_CONFIG, multimedia) {
        SUBDIRS += \
                qgraphicsvideoitem \
                qpaintervideosurface

    }

    symbian: {
        #symbian spesific autotests
        SUBDIRS += symbian 
        SUBDIRS -= \
                qcamera \
                qmediaplayer \
                qradiotuner \
                qmediaobject
    }
}
#Messaging
contains(mobility_modules,messaging) {
    contains(qmf_enabled,yes)|wince*|win32|symbian|maemo6 {
    !win32-g++:SUBDIRS += \
        qmessagestore \
        qmessagestorekeys \
        qmessage \
        qmessageservice
    }
}
