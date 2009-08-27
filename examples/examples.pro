TEMPLATE = subdirs
SUBDIRS += simple logfilepositionsource

contains(QT_CONFIG, webkit) {
    SUBDIRS += fetchgooglemaps
}
