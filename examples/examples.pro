TEMPLATE = subdirs

SUBDIRS += filemanagerplugin \
           bluetoothtransferplugin \
           servicebrowser

SUBDIRS += bearermonitor bearercloud
contains(QT_CONFIG, declarative) {
    SUBDIRS += declarative
}
