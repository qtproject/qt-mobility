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
      qlandmarkmanager

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
