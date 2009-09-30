TEMPLATE = subdirs

CONFIG += ordered

#sfw unit tests dont work yet on Symbian. enable later
!symbian {
    SUBDIRS += servicemetadata \            #service framework
           servicedatabase \
           databasemanager \
           qserviceinterfacedescriptor \
           qservicefilter \
           qservicemanager \
           qabstractsecuritysession \
           qservicecontext 
}
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

!wince:SUBDIRS += qvaluespace \                    #Publish and Subscribe
           qvaluespaceobject \
           qvaluespaceitem \
           qmallocpool

unix:SUBDIRS += qpacket \
                qpacketprotocol \
                qsystemreadwritelock \
                qsystemreadwritelock_oop

!symbian:SUBDIRS += qsysteminfo \                    #SystemInformation
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
