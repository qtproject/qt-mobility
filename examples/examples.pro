TEMPLATE = subdirs

SUBDIRS += filemanagerplugin \
           bluetoothtransferplugin \
           servicebrowser

contains(QT_CONFIG, declarative) {
    SUBDIRS += declarative
}
