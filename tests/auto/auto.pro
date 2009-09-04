TEMPLATE = subdirs
SUBDIRS = qgeocoordinate \
          qgeopositioninfo \
          qsatelliteinfo \
          qsatelliteinfosource \
          qgeopositioninfosource \
          qgeoareamonitor \
          qlocationutils \
          qnmeapositioninfosource 

symbian {          
    SUBDIRS += qgeopositioninfosource_s60 
}

