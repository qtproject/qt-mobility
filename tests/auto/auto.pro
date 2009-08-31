TEMPLATE = subdirs
SUBDIRS += servicemetadata \                #service framework
           servicedatabase \
           databasemanager \
           qserviceinterfacedescriptor \
           qservicefilter \
           qservicemanager \
           qabstractsecuritysession \
           qservicecontext \
           qnetworkconfigmanager \          #Bearer management
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
           tst_qnmeapositioninfosource_sim 
