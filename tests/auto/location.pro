include(../../staticconfig.pri)

requires(contains(mobility_modules,location))

TEMPLATE = subdirs
SUBDIRS += qgeocoordinate \
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
      qlandmarkcategory \
      qlandmarkmanager \
      qgeoaddress \
      qgeomaneuver \
      qgeoplace \
      qgeoroute \
      qgeoroutingmanagerplugins \
      qgeoroutingmanager \
      qgeoroutereply \
      qgeorouterequest \
      qgeoroutesegment \
      qgeosearchreply \
      qgeosearchmanagerplugins \
      qgeosearchmanager \
      qgeoserviceproviderplugins \
      qgeoserviceprovider

#IGNORED_UNTIL_INTEGRATED_PROPERLY += \
#    qdeclarativeapitests \
#    geoservicesgeomapplugin \
#    geoservicesgeotiledmapplugin \
#    qgeocoordinate2 \
#    qgeomapobjects \
#    qgeopositioninfo2

contains(QT_CONFIG, declarative) {
    SUBDIRS += qdeclarativeposition
    SUBDIRS += qdeclarativelandmark
}
wince* {
    SUBDIRS += qgeoinfosources_wince
}

SUBDIRS +=  qlandmarkfilehandler_gpx \
            qlandmarkfilehandler_lmx

