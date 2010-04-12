#####################################################################
# Main projectfile
#####################################################################

CONFIG += ordered 
TEMPLATE = subdirs

SUBDIRS = demoapp\
          tst_qgeopositioninfosource \
          tst_qgeocoordinate\
          tst_qgeopositioninfo\
          tst_qsatelliteinfo\
          tst_qsatelliteinfosource

include(shell.pri)

