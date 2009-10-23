TEMPLATE = subdirs
SUBDIRS += auto
!symbian:unix:!mac:SUBDIRS += networkmanager
