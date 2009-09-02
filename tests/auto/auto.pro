TEMPLATE = subdirs
SUBDIRS = qgeocoordinate \
          qgeopositioninfo \
          qsatelliteinfo \
          qsatelliteinfosource \
          qgeopositioninfosource \
          qgeoareamonitor \
          qlocationutils \
          qnmeapositioninfosource \
          qnmeapositioninfosource_realtime \
          qnmeapositioninfosource_sim 

symbian {          
    SUBDIRS += qgeopositioninfosource_s60 
}

