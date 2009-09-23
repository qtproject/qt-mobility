TEMPLATE = subdirs
SUBDIRS += auto testservice2 sampleserviceplugin sampleserviceplugin2
!symbian:!mac:unix:SUBDIRS += networkmanager
symbian:SUBDIRS += bearerex
