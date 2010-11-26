include(../../staticconfig.pri)

requires(contains(mobility_modules,location))

TEMPLATE = subdirs
CONFIG += ordered
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
#    qgeomapobjects

contains(QT_CONFIG, declarative) {
    SUBDIRS += qdeclarativeposition
    SUBDIRS += qdeclarativelandmark
}
wince* {
    SUBDIRS += qgeoinfosources_wince
}

!symbian{
SUBDIRS +=  qlandmarkfilehandler_gpx \
            qlandmarkfilehandler_lmx
}
