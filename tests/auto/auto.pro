TEMPLATE = subdirs
SUBDIRS = qgeocoordinate \
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

