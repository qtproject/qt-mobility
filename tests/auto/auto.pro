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

SUBDIRS += tst_qgeocoordinate \             #Location
           tst_qgeopositioninfo \
           tst_qsatelliteinfo \
           tst_qsatelliteinfosource \
           tst_qgeopositioninfosource \
           tst_qgeoareamonitor \
           tst_qlocationutils \
           tst_qnmeapositioninfosource \
           tst_qnmeapositioninfosource_realtime \
           tst_qnmeapositioninfosource_sim

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
          qsystemmemoryinfo \
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

