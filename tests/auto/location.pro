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
      qlandmarkmanagerengine \
      qlandmark \
      qlandmarkcategory \
      qlandmarkmanager \
      qgeoaddress \
      qgeomaneuver \
      qgeomapobjectplugin \
      qgeomapcircleobject \
      qgeomapgroupobject \
      qgeomappixmapobject \
      qgeomappolygonobject \
      qgeomappolylineobject \
      qgeomaprectangleobject \
      qgeomaprouteobject \
      qgeomaptextobject \
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
      qgeoserviceprovider \
      geoservicesgeomapplugin \
      geoservicesgeomap \
      geoservicesgeotiledmapplugin \
      geoservicesgeotiledmap


#IGNORED_UNTIL_INTEGRATED_PROPERLY += \
#    qdeclarativeapitests


SUBDIRS +=qlandmarkmanagerplugins

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
