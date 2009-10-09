TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += databasemanager \                #service framework
           servicedatabase \
           servicemetadata \
           qserviceinterfacedescriptor \
           qservicefilter \
           qservicemanager \
           qabstractsecuritysession \
           qservicecontext

SUBDIRS += qnetworkconfigmanager \          #Bearer management
           qnetworkconfiguration \
           qnetworksession \

SUBDIRS += qgeocoordinate \                 #Location
          qgeopositioninfo \
          qgeosatelliteinfo \
          qgeosatelliteinfosource \
          qgeopositioninfosource \
          qgeoareamonitor \
          qlocationutils \
          qnmeapositioninfosource 
symbian:SUBDIRS += qgeopositioninfosource_s60 

SUBDIRS += qvaluespace \                           #Publish and Subscribe
           qvaluespaceprovider \
           qvaluespacesubscriber \
           qmallocpool \
           qpacket \
           qpacketprotocol

unix:!symbian:SUBDIRS += \
                qsystemreadwritelock \
                qsystemreadwritelock_oop

!symbian:!wince*:SUBDIRS += qsysteminfo \                    #SystemInformation
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
            qcontactgroup \
            qcontactmanager \
            qcontactmanagerplugins \
            qcontactmanagerfiltering

SUBDIRS += \           #Multimedia
        qabstractmediaobject \
        qabstractmediaservice \
        qmediaplaylist \
        qmediaplaylistnavigator \
        qmediaresource \
        qmediasource \
        qvideowidget \
        qmediaplayer \
        qradioplayer \
        qmediarecorder \
        qaudiosource \
        qcamera

#Messaging
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo {
    SUBDIRS += \
        support \
        qmessagestore \
        qmessagestorekeys \
        qmessage
}
