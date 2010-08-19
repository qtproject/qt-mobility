TEMPLATE = subdirs

include(../../staticconfig.pri)

#SUBDIRS += headers

contains(mobility_modules,serviceframework) {
    SUBDIRS += servicemetadata \                   #service framework
           qserviceinterfacedescriptor \
           qservicefilter \
           qservicemanager \
           qabstractsecuritysession \
           qservicecontext \
           qmetaobjectbuilder \
           icheck \
           qservicemanager_ipc \
           qremoteserviceclassregister \
           servicedatabase
         # databasemanager # disabled from auto builds

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
          qnmeapositioninfosource \
          #qlandmarkmanagerplugins \
          qlandmarkmanagerengine \
          qlandmark \
          qlandmarkcategory

     contains(QT_CONFIG, declarative) {
         SUBDIRS += qdeclarativeposition
     }
    wince* {
        SUBDIRS += qgeoinfosources_wince
    }

    SUBDIRS +=  qlandmarkfilehandler_gpx
                #qlandmarkfilehandler_lmx
    
#disable for now    SUBDIRS += qlandmarkmanagerengine_sqlite
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
    maemo6: {
          #maemo6 spesific autotests
          SUBDIRS += maemo6
    }
}

contains(mobility_modules,contacts) {
    #Contacts
    SUBDIRS +=  qcontact \
            qcontactasync \
            qcontactdetail \
            qcontactdetaildefinition \
            qcontactdetails \
            qcontactfilter \
            qcontactmanager \
            qcontactmanagerplugins \
            qcontactrelationship \
            qlatin1constant
    # This needs glibc:
    linux*: SUBDIRS += qcontactmemusage

    contains(mobility_modules,serviceframework){
            SUBDIRS += qcontactmanagerfiltering \
            qcontactactions
    } else: warning(Some contacts unit tests depend on service framework too)

}

contains(mobility_modules,organizer) {
    # Organizer
    SUBDIRS += qorganizeritemmanager
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

    contains(mobility_modules,organizer) {
        SUBDIRS += \
            qversitorganizerexporter \
            qversitorganizerimporter
    }
}

contains(mobility_modules,telephony) {
    # TODO change this when other backends are developed
    linux-*: SUBDIRS += qtelephony
}

contains(mobility_modules,multimedia) {
    # Multimedia
    SUBDIRS += \
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
        qcamera \
        qpaintervideosurface \
        qvideowidget \
        qmediatimerange \
        qaudiodeviceinfo \
        qaudiooutput \
        qaudioinput \
        qaudioformat \
        qvideoframe \
        qvideosurfaceformat
}
#Messaging
contains(mobility_modules,messaging) {
    contains(qmf_enabled,yes)|wince*|win32|symbian|maemo5|maemo6 {
    !win32-g++:SUBDIRS += \
        qmessagestore \
        qmessagestorekeys \
        qmessage \
        qmessageservice
    }
}

# Sensors
contains(mobility_modules,sensors) {
    SUBDIRS += qsensor
}

#Document Gallery
contains(mobility_modules,gallery) {
    SUBDIRS += \
        qdocumentgallery \
        qgalleryabstractrequest \
        qgalleryabstractresponse \
        qgalleryitemrequest \
        qgalleryquerymodel \
        qgalleryqueryrequest \
        qgalleryremoverequest \
        qgalleryresource \
        qgallerytyperequest

    !unix: SUBDIRS += qgalleryfilter

    unix: contains(QT_CONFIG, dbus): SUBDIRS += qgallerytrackerresultset_maemo5
}

