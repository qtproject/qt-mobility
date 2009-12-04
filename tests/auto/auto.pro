TEMPLATE = subdirs

include($$QT_MOBILITY_BUILD_TREE/config.pri)

#SUBDIRS += databasemanager \                #service framework
SUBDIRS += servicemetadata \
           qserviceinterfacedescriptor \
           qservicefilter \
#           qservicemanager \  #remove until qhash namespace issue resolved
           qabstractsecuritysession \
           qservicecontext

# servicedatabase is not compiled into the serviceframework library on symbian,
# special handling is needed
!symbian:SUBDIRS+=servicedatabase

SUBDIRS += qnetworkconfigmanager \          #Bearer management
           qnetworkconfiguration \
           qnetworksession

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

SUBDIRS += qvaluespace \                           #Publish and Subscribe
           qvaluespacepublisher \
           qvaluespacesubscriber \
	   qcrmlparser

unix|win32 {
    !symbian:!maemo: SUBDIRS+= \
        qsystemreadwritelock \
        qsystemreadwritelock_oop
}

unix:!symbian:!maemo: {
    SUBDIRS+= \
           qpacket \
           qmallocpool \
           qpacketprotocol
}

!maemo:SUBDIRS += qsysteminfo \                    #SystemInformation
          qsystemdeviceinfo \
          qsystemdisplayinfo \
          qsystemstorageinfo \
          qsystemnetworkinfo \
          qsystemscreensaver

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

# Versit module
SUBDIRS += \
            qvcard21writer \
            qvcard30writer \
            qversitcontactexporter \
            qversitcontactimporter \
            qversitdocument \
            qversitproperty \
            qversitreader \
            qversitutils \
            qversitwriter


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
        qvideowidget

contains(QT_CONFIG, multimedia) {
    SUBDIRS += \
            qgraphicsvideoitem \
            qpaintervideosurface

}

#Messaging
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo {
    !win32-g++:SUBDIRS += \
        qmessagestore \
        qmessagestorekeys \
        qmessage
}
