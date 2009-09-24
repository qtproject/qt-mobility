TEMPLATE = subdirs
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
           tst_qgeocoordinate \             #Location
           tst_qgeopositioninfo \
           tst_qsatelliteinfo \
           tst_qsatelliteinfosource \
           tst_qgeopositioninfosource \
           tst_qgeoareamonitor \
           tst_qlocationutils \
           tst_qnmeapositioninfosource \
           tst_qnmeapositioninfosource_realtime \
           tst_qnmeapositioninfosource_sim \ 
           qvaluespace \                    #Publish and Subscribe
           qvaluespaceobject \
           qvaluespaceitem \
           qmallocpool

unix:SUBDIRS += qpacket \
                qpacketprotocol \
                qsystemreadwritelock \
                qsystemreadwritelock_oop

SUBDIRS += qsysteminfo \                    #SystemInformation
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
