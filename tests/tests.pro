TEMPLATE = subdirs
SUBDIRS += auto testservice2 sampleserviceplugin sampleserviceplugin2
!symbian:unix:SUBDIRS += networkmanager
symbian:SUBDIRS += bearerex
