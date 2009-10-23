TEMPLATE = subdirs
SUBDIRS = qgeocoordinate \
          qgeopositioninfo \
          qgeosatelliteinfo \
          qgeosatelliteinfosource \
          qgeopositioninfosource \
          qgeoareamonitor \
          qlocationutils \
          qnmeapositioninfosource 

symbian {          
    SUBDIRS += qgeopositioninfosource_s60 
}

wince* {
    SUBDIRS += qgeoinfosources_wince
}

