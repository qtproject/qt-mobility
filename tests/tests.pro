TEMPLATE = subdirs
SUBDIRS += auto 
!symbian:unix:SUBDIRS += networkmanager
maemo:SUBDIRS -= networkmanager
