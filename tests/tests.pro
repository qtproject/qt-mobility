TEMPLATE = subdirs
SUBDIRS += auto
unix:!symbian:!maemo:!mac:SUBDIRS += networkmanager
