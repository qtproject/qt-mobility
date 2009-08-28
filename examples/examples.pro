TEMPLATE = subdirs
SUBDIRS += logfilepositionsource

contains(QT_CONFIG, webkit) {
    SUBDIRS += fetchgooglemaps
}
